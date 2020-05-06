#pragma once

//  .------------------------.      .--------------.
//  | worker_thread_upstream |  ->  | worker_group |
//  .------------------------.      .--------------.
//      ^       ^       ^
//      |       |       |
//  .------------------------.
//  |     worker_threads     |
//  .------------------------.

#include <functional>
#include <memory_resource>
#include <thread>
#include "igiaccleration/circular_list.h"
#include "igiaccleration/mem_arena.h"

namespace igi {
    template <typename TaskOut, typename TaskIn>
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

            bool retrieve_lock(TaskIn *in) { return _upstream.retrieve_lock(in); }

            void submit(TaskOut &&out) { _upstream.submit(std::move(out)); }

            operator bool() { return _upstream._running; }
        };

        friend class token;

        using allocator_type = std::pmr::polymorphic_allocator<TaskIn>;

        worker_thread_upstream(const std::function<void(TaskOut &&)> &callback, size_t maxQueue,
                               const allocator_type &alloc)
            : _running(true), _activeCount(0), _callback(callback), _queue(maxQueue, alloc) { }

        token getToken() {
            return token(*this);
        }

        void waitFinish() {
            std::thread([&] {
                _running = false;
                while (_activeCount)
                    std::this_thread::yield();
            }).join();
        }

        template <typename... T>
        bool tryIssue(T &&... in) {
            if (!_queue.isFull()) {
                _queue.emplace_back(std::forward<T>(in)...);
                return true;
            }
            return false;
        }

      private:
        bool retrieve_lock(TaskIn *in) {
            std::scoped_lock sl(_mutex);
            if (!_queue.isEmpty()) {
                *in = _queue.pop_front();
                return true;
            }
            return false;
        }

        void submit(TaskOut &&out) { _callback(std::move(out)); }

        bool _running;
        size_t _activeCount;
        std::function<void(TaskOut &&)> _callback;
        circular_list<TaskIn> _queue;
        std::mutex _mutex;
    };

    template <typename TaskOut, typename TaskIn>
    class worker_thread {
      public:
        using task_upstream_t = worker_thread_upstream<TaskOut, TaskIn>;
        using worker_func_t   = std::function<TaskOut(TaskIn &)>;

        worker_thread(task_upstream_t &upstream, const worker_func_t &workerFunc)
            : _upstream(upstream), _thread(wrapWorkerFunc(workerFunc)) { }

        void detach() { _thread.detach(); }

      private:
        task_upstream_t &_upstream;
        std::thread _thread;

        auto wrapWorkerFunc(const worker_func_t &func) {
            return [&, func] {
                TaskIn in;
                while (true) {
                    auto t = _upstream.getToken();
                    if (!t) break;
                    if (t.retrieve_lock(&in))
                        t.submit(func(in));
                }
            };
        }
    };

    template <typename TaskOut, typename TaskIn>
    class worker_group {
      public:
        using worker_thread_t = worker_thread<TaskOut, TaskIn>;
        using worker_func_t   = typename worker_thread_t::worker_func_t;
        using worker_alloc_t  = std::pmr::polymorphic_allocator<worker_thread_t>;
        using callback_t      = std::function<void(TaskOut &&)>;

        worker_group(const callback_t &callback, const worker_func_t &workerFunc,
                     size_t maxQueue, size_t workerCount, worker_alloc_t &alloc)
            : _workers(alloc.allocate(workerCount)), _workerCount(workerCount),
              _upstream(callback, maxQueue, alloc), _alloc(alloc) {
            for (size_t i = 0; i < workerCount; i++)
                _alloc.construct(&_workers[i], _upstream, workerFunc);
        }

        ~worker_group() {
            for (size_t i = 0; i < _workerCount; i++)
                _workers[i].~worker_thread();
            _alloc.deallocate(_workers, _workerCount);
        }

#pragma region Static Functions

        static worker_group *DetachN(const callback_t &callback, const worker_func_t &workerFunc,
                                     size_t maxQueue, size_t threadCount, const worker_alloc_t &alloc) {
            using group_alloc_t = std::pmr::polymorphic_allocator<worker_group>;

            group_alloc_t groupAlloc(alloc);
            worker_group *group = groupAlloc.allocate(1);
            groupAlloc.construct(group, callback, workerFunc, maxQueue, threadCount, alloc);
            group->detachAll();

            return group;
        }

        static worker_group *DetachMax(const callback_t &callback, const worker_func_t &workerFunc,
                                       size_t maxQueue, const worker_alloc_t &alloc) {
            return DetachN(callback, workerFunc, maxQueue, std::thread::hardware_concurrency(), alloc);
        }

#pragma endregion

        void detachAll() {
            for (size_t i = 0; i < _workerCount; i++)
                _workers[i].detach();
        }

        template <typename... T>
        bool tryIssue(T &&... in) {
            return _upstream.tryIssue(std::forward<T>(in)...);
        }

        void waitFinish() {
            _upstream.waitFinish();
        }

      private:
        worker_thread_t *_workers;
        size_t _workerCount;
        worker_thread_upstream<TaskOut, TaskIn> _upstream;
        worker_alloc_t _alloc;
    };
}  // namespace igi
