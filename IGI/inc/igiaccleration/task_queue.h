#pragma once

#include <functional>
#include <memory_resource>
#include <thread>
#include "igiaccleration/circular_list.h"
#include "igiaccleration/mem_arena.h"

namespace igi {
    template <typename TaskOut, typename TaskIn>
    class worker_thread {
      public:
        using task_queue_t  = circular_list<TaskIn>;
        using worker_func_t = std::function<TaskOut(TaskIn &)>;
        using task_alloc_t  = std::pmr::polymorphic_allocator<TaskIn>;

        class worker_group {
            std::mutex _mutex;
            task_queue_t _taskQueue;
            worker_thread *_workers;

          public:
            worker_group(const worker_func_t &workerFunc, size_t maxQueue,
                         size_t workerCount, const task_alloc_t &alloc) : _taskQueue(maxQueue, alloc) {
                std::pmr::polymorphic_allocator<worker_thread> workerAlloc(alloc);
                _workers = workerAlloc.allocate(workerCount);

                for (size_t i = 0; i < workerCount; i++)
                    workerAlloc.construct(&_workers[i], _taskQueue, workerFunc);
            }

            bool issueTask(const TaskIn &task) {
                {
                    std::scoped_lock sl(_mutex);
                    if (!_taskQueue.isFull())
                        _taskQueue.push_back(task);
                    else
                        return false;
                }
                return true;
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

      private:
        task_queue_t &_taskQueue;
        std::thread _thread;
    };
}  // namespace igi