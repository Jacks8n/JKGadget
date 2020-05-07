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

        using allocator_t = std::pmr::polymorphic_allocator<TTask>;

        worker_thread_upstream(size_t maxQueue, const allocator_t &alloc)
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

        worker_thread(task_upstream_t &upstream, const worker_func_t &workerFunc)
            : _upstream(upstream), _thread(wrapWorkerFunc(workerFunc)) { }

      private:
        task_upstream_t &_upstream;
        std::thread _thread;

        auto wrapWorkerFunc(const worker_func_t &func) {
            return [&, func] {
                worker_input_t in;
                while (true) {
                    auto t = _upstream.getToken();
                    if (!t) break;
                    if (RetrieveInput(t, &in))
                        func(in);
                    else
                        std::this_thread::yield();
                }
            };
        }

        static bool RetrieveInput(typename task_upstream_t::token &token, worker_input_t *in) {
            if constexpr (std::is_same_v<TContext, void>)
                return token.retrieve_lock(in);
            else
                return token.retrieve_lock(&in->first);
        }
    };

    template <typename TTask, typename TContext = void>
    class worker_group {
      public:
        using worker_thread_t = worker_thread<TTask, TContext>;
        using worker_func_t   = typename worker_thread_t::worker_func_t;
        using allocator_t     = std::pmr::polymorphic_allocator<worker_thread_t>;

        using upstream_t = worker_thread_upstream<TTask>;

        worker_group(const worker_func_t &workerFunc, size_t maxQueue,
                     size_t workerCount, allocator_t alloc)
            : _workers(alloc.allocate(workerCount)), _workerCount(workerCount),
              _upstream(maxQueue, alloc), _alloc(std::move(alloc)) {
            for (size_t i = 0; i < workerCount; i++)
                _alloc.construct(&_workers[i], _upstream, workerFunc);
        }

        ~worker_group() {
            for (size_t i = 0; i < _workerCount; i++)
                _workers[i].~worker_thread();
            _alloc.deallocate(_workers, _workerCount);
        }

#pragma region Static Functions

        template <typename TAlloc>
        static worker_group *DetachN(const worker_func_t &workerFunc, size_t maxQueue,
                                        size_t threadCount, TAlloc &&alloc) {
            using group_alloc_t = std::pmr::polymorphic_allocator<worker_group>;

            group_alloc_t groupAlloc(alloc);
            worker_group *group = groupAlloc.allocate(1);
            groupAlloc.construct(group, workerFunc, maxQueue, threadCount, std::forward<TAlloc>(alloc));

            return group;
        }

        template <typename TAlloc>
        static worker_group *DetachMax(const worker_func_t &workerFunc, size_t maxQueue,
                                          TAlloc &&alloc) {
            return DetachN(workerFunc, maxQueue, std::thread::hardware_concurrency(),
                              std::forward<TAlloc>(alloc));
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
        worker_thread_t *_workers;
        size_t _workerCount;
        upstream_t _upstream;
        allocator_t _alloc;
    };
}  // namespace igi
