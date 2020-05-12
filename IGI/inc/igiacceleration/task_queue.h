#pragma once

//  .------------------------.      .--------------.
//  | worker_thread_upstream |  ->  | worker_group |
//  .------------------------.      .--------------.
//      ^       ^       ^
//      |       |       |
//  .------------------------.
//  |     worker_thread      |
//  .------------------------.

#include <functional>
#include <memory_resource>
#include <thread>
#include "igiacceleration/circular_list.h"
#include "igiacceleration/mem_arena.h"

namespace igi {
    template <typename TTask>
    class worker_thread_upstream {
      public:
        class token {
            worker_thread_upstream &_upstream;

          public:
            token(worker_thread_upstream &upstream) : _upstream(upstream) {
                std::scoped_lock sl(_upstream._mutex);
                _upstream._activeCount++;
            }

            ~token() {
                std::scoped_lock sl(_upstream._mutex);
                _upstream._activeCount--;
            }

            bool retrieve_lock(TTask *in) { return _upstream.retrieve_lock(in); }

            operator bool() { return _upstream._running; }
        };

        friend class token;

        using allocator_type = std::pmr::polymorphic_allocator<TTask>;

        worker_thread_upstream(size_t maxQueue, const allocator_type &alloc)
            : _running(true), _activeCount(0), _queue(maxQueue, alloc) { }

        token getToken() {
            return token(*this);
        }

        void finishNow() {
            std::thread([&] {
                _running = false;
                while (_activeCount)
                    std::this_thread::yield();
            }).join();
        }

        void waitFinish() {
            std::thread([&] {
                while (!_queue.isEmpty())
                    std::this_thread::yield();
                _running = false;
                while (_activeCount)
                    std::this_thread::yield();
            }).join();
        }

        template <typename... T>
        void issue(T &&... in) {
            while (true) {
                if (!_queue.isFull()) {
                    _queue.emplace_back(std::forward<T>(in)...);
                    break;
                }
                std::this_thread::yield();
            }
        }

      private:
        bool retrieve_lock(TTask *in) {
            std::scoped_lock sl(_mutex);
            if (!_queue.isEmpty()) {
                new (in) TTask(std::move(_queue.pop_front()));
                return true;
            }
            return false;
        }

        bool _running;
        size_t _activeCount;
        circular_list<TTask> _queue;
        std::mutex _mutex;
    };

    template <typename TTask, typename TContext = void>
    class worker_thread {
        template <typename T>
        struct worker_input { using type = std::pair<TTask, TContext>; };
        template <>
        struct worker_input<void> { using type = TTask; };

      public:
        using task_upstream_t = worker_thread_upstream<TTask>;

        using worker_input_t = typename worker_input<TContext>::type;
        using worker_func_t  = std::function<void(worker_input_t &)>;

        template <typename... TContextArgs>
        worker_thread(task_upstream_t &upstream, const worker_func_t &workerFunc,
                      TContextArgs &&... contextArgs)
            : _thread(worker_func_wrapper(
                upstream, workerFunc, std::forward<TContextArgs>(contextArgs)...)) { }

      private:
        std::thread _thread;

        class worker_func_wrapper {
            task_upstream_t &_upstream;
            const worker_func_t &_func;
            worker_input_t _in;

          public:
            worker_func_wrapper(task_upstream_t &upstream, const worker_func_t &workerFunc)
                : _upstream(upstream), _func(workerFunc), _in() { }

            template <typename... TContextArgs,
                      std::enable_if_t<!std::is_same_v<TContext, void>, int> = 0>
            worker_func_wrapper(task_upstream_t &upstream, const worker_func_t &workerFunc,
                                TContextArgs &&... args)
                : _upstream(upstream), _func(workerFunc),
                  _in(TTask(), TContext(std::forward<TContextArgs>(args)...)) { }

            void operator()() {
                while (true) {
                    auto t = _upstream.getToken();
                    if (!t) break;
                    if (RetrieveInput(t, &_in))
                        _func(_in);
                    else
                        std::this_thread::yield();
                }
            }

          private:
            static bool RetrieveInput(typename task_upstream_t::token &token, worker_input_t *in) {
                if constexpr (std::is_same_v<TContext, void>)
                    return token.retrieve_lock(in);
                else
                    return token.retrieve_lock(&in->first);
            }
        };
    };

    template <typename TTask, typename TContext = void>
    class worker_group {
      public:
        using worker_thread_t      = worker_thread<TTask, TContext>;
        using worker_func_t        = typename worker_thread_t::worker_func_t;
        using allocator_type       = std::pmr::polymorphic_allocator<worker_thread_t>;
        using group_allocator_type = std::pmr::polymorphic_allocator<worker_group>;

        using upstream_t = worker_thread_upstream<TTask>;

        template <typename... TContextArgs>
        worker_group(const worker_func_t &workerFunc, size_t maxQueue, size_t workerCount,
                     const allocator_type &alloc, TContextArgs &&... contextArgs)
            : _alloc(alloc), _workers(_alloc.allocate(workerCount)),
              _workerCount(workerCount), _upstream(maxQueue, alloc) {
            for (size_t i = 0; i < workerCount; i++)
                _alloc.construct(&_workers[i], _upstream, workerFunc,
                                 std::forward<TContextArgs>(contextArgs)...);
        }

        ~worker_group() {
            for (size_t i = 0; i < _workerCount; i++)
                _workers[i].~worker_thread();
            _alloc.deallocate(_workers, _workerCount);
        }

#pragma region Static Functions

        template <typename... TContextArgs>
        static worker_group *DetachN(const worker_func_t &workerFunc, size_t maxQueue, size_t workerCount,
                                     const group_allocator_type &alloc, TContextArgs &&... contextArgs) {
            group_allocator_type alloctmp(alloc);
            worker_group *group = alloctmp.allocate(1);
            new (group) worker_group(workerFunc, maxQueue, workerCount, static_cast<allocator_type>(alloc),
                                     std::forward<TContextArgs>(contextArgs)...);
            return group;
        }

        template <typename... TContextArgs>
        static worker_group *DetachMax(const worker_func_t &workerFunc, size_t maxQueue,
                                       const group_allocator_type &alloc, TContextArgs &&... contextArgs) {
            return DetachN(workerFunc, maxQueue, std::thread::hardware_concurrency(),
                           alloc, std::forward<TContextArgs>(contextArgs)...);
        }

#pragma endregion

        size_t getWorkerCount() { return _workerCount; }

        template <typename... T>
        void issue(T &&... in) {
            return _upstream.issue(std::forward<T>(in)...);
        }

        void finishNow() {
            _upstream.finishNow();
        }

        void waitFinish() {
            _upstream.waitFinish();
        }

      private:
        allocator_type _alloc;
        worker_thread_t *_workers;
        size_t _workerCount;
        upstream_t _upstream;
    };
}  // namespace igi
