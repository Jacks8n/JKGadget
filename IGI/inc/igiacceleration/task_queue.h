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

        worker_thread_upstream(size_t maxQueue)
            : _running(true), _activeCount(0), _queue(maxQueue, context::GetTypedAllocator<TTask>()) {
            _queue.clear();
        }

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
                while (!_queue.empty())
                    std::this_thread::yield();
                _running = false;
                while (_activeCount)
                    std::this_thread::yield();
            }).join();
        }

        template <typename... T>
        void issue(T &&...in) {
            while (true) {
                if (!_queue.full()) {
                    _queue.emplace_back(std::forward<T>(in)...);
                    break;
                }
                std::this_thread::yield();
            }
        }

      private:
        bool retrieve_lock(TTask *in) {
            std::scoped_lock sl(_mutex);
            if (!_queue.empty()) {
                new (in) TTask(_queue.front());
                _queue.pop_front();
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
                      TContextArgs &&...contextArgs)
            : _thread(worker_func_wrapper(
                upstream, workerFunc, std::forward<TContextArgs>(contextArgs)...)) { }

      private:
        std::thread _thread;

        class worker_func_wrapper {
            task_upstream_t &_upstream;
            worker_func_t _func;
            worker_input_t _in;

          public:
            worker_func_wrapper(task_upstream_t &upstream, const worker_func_t &workerFunc)
                : _upstream(upstream), _func(workerFunc), _in() { }

            template <typename... TContextArgs,
                      std::enable_if_t<!std::is_same_v<TContext, void>, int> = 0>
            worker_func_wrapper(task_upstream_t &upstream, const worker_func_t &workerFunc,
                                TContextArgs &&...args)
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
        using worker_thread_t = worker_thread<TTask, TContext>;
        using worker_func_t   = typename worker_thread_t::worker_func_t;

        using upstream_t = worker_thread_upstream<TTask>;

        template <typename... TContextArgs>
        worker_group(const worker_func_t &workerFunc, size_t maxQueue, size_t workerCount, TContextArgs &&...contextArgs)
            : _workers(context::Allocate<worker_thread_t>(workerCount)), _workerCount(workerCount),
              _upstream(maxQueue) {
            for (size_t i = 0; i < workerCount; i++)
                context::Construct(&_workers[i], _upstream, workerFunc, std::forward<TContextArgs>(contextArgs)...);
        }

        ~worker_group() {
            context::Destroy(_workers, _workerCount);
            context::Allocate.deallocate(_workers, _workerCount);
        }

#pragma region Static Functions

        template <typename... TContextArgs>
        static worker_group *DetachN(const worker_func_t &workerFunc, size_t maxQueue, size_t workerCount, TContextArgs &&...contextArgs) {
            worker_group *group = context::Allocate<worker_group>();
            context::Construct(group, workerFunc, maxQueue, workerCount, std::forward<TContextArgs>(contextArgs)...);
            return group;
        }

        template <typename... TContextArgs>
        static worker_group *DetachMax(const worker_func_t &workerFunc, size_t maxQueue, TContextArgs &&...contextArgs) {
            size_t threadCount;

#ifdef _DEBUG
            threadCount = 1;
#else
            threadCount = std::thread::hardware_concurrency();
#endif

            return DetachN(workerFunc, maxQueue, threadCount, std::forward<TContextArgs>(contextArgs)...);
        }

#pragma endregion

        size_t getWorkerCount() { return _workerCount; }

        template <typename... T>
        void issue(T &&...in) {
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
    };
}  // namespace igi
