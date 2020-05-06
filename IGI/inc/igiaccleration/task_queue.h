#pragma once

#include <functional>
#include <memory_resource>
#include <thread>
#include "igiaccleration/circular_list.h"
#include "igiaccleration/mem_arena.h"

namespace igi {
    template <typename TaskOut, typename TaskIn>
    class worker_thread {
        using task_queue_t  = circular_list<TaskIn>;
        using worker_func_t = std::function<TaskOut(TaskIn &)>;
        using task_alloc_t  = std::pmr::polymorphic_allocator<TaskIn>;

        task_queue_t &_taskQueue;
        std::thread _thread;

      public:
        class worker_group {
            using worker_alloc_t = std::pmr::polymorphic_allocator<worker_thread>;

            std::mutex _mutex;
            task_queue_t _taskQueue;
            worker_thread *_workers;
            size_t _workerCount;
            worker_alloc_t _alloc;

          public:
            worker_group(const worker_func_t &workerFunc, size_t maxQueue,
                         size_t workerCount, const worker_alloc_t &alloc)
                : _taskQueue(maxQueue, alloc), _workerCount(workerCount), _alloc(alloc) {
                _workers = _alloc.allocate(workerCount);

                for (size_t i = 0; i < workerCount; i++)
                    _alloc.construct(&_workers[i], _taskQueue, workerFunc);
            }

            ~worker_group() {
                _taskQueue.~circular_list();
                for (size_t i = 0; i < _workerCount; i++)
                    _workers[i].~worker_thread();
                _alloc.deallocate(_workers, _workerCount);
            }

            template <typename T>
            bool issue(T &&in) {
                std::scoped_lock sl(_mutex);
                if (!_taskQueue.isFull())
                    _taskQueue.emplace_back(std::forward<T>(in)...);
                else
                    return false;
                return true;
            }

            bool retrieve(TaskOut *out) {
                std::scoped_lock sl(_mutex);
                *out = _taskQueue.pop_front();
                return false;
            }
        };

        worker_thread(task_queue_t &taskQueue, const worker_func_t &workerFunc)
            : _tasks(taskQueue), _thread(workerFunc) { }

        static worker_group *DetachN(const worker_func_t &workerFunc, const task_alloc_t &alloc,
                                     size_t maxQueue, size_t threadCount) {
            using group_alloc_t = std::pmr::polymorphic_allocator<worker_group>;

            group_alloc_t groupAlloc(alloc);
            worker_group *group = groupAlloc.allocate(1);
            groupAlloc.construct(group, maxQueue, threadCount, alloc);

            return group;
        }

        static worker_group *DetachMax(const worker_func_t &workerFunc, const task_alloc_t &alloc,
                                       size_t maxQueue) {
            return DetachN(workerFunc, alloc, maxQueue, std::thread::hardware_concurrency());
        }
    };
}  // namespace igi
