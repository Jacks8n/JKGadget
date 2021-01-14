#pragma once

#include <atomic>
#include <functional>
#include <memory_resource>
#include <thread>
#include "igiacceleration/mem_arena.h"

namespace igi {
    template <typename TContext>
    struct parallel_traits {
        using context_ctor_t = std::function<TContext()>;

        static inline const size_t DefaultTaskBufferSize = std::thread::hardware_concurrency() * 4;

        static inline const size_t DefaultConsumerCount = std::max(std::thread::hardware_concurrency(), 2u) - 1;
    };

    template <typename TContext, typename... TJobArgs>
    class parallel_job;

    template <typename TContext>
    class parallel_context : private parallel_traits<TContext> {
        using typename parallel_traits<TContext>::context_ctor_t;
        using parallel_traits<TContext>::DefaultTaskBufferSize;
        using parallel_traits<TContext>::DefaultConsumerCount;

        context_ctor_t _contextCtor;

      public:
        parallel_context() : parallel_context([]() { return TContext(); }) { }

        template <typename TCtor>
        parallel_context(TCtor &&contextCtor)
            : _contextCtor(std::forward<TCtor>(contextCtor)) { }

        template <typename... TArgs>
        parallel_job<TContext, TArgs...> schedule(void (*func)(TContext &, TArgs...), size_t consumerCount = 0, size_t bufferSize = 0) {
            if (!bufferSize)
                bufferSize = DefaultTaskBufferSize;
            if (!consumerCount)
                consumerCount = DefaultConsumerCount;

            return parallel_job<TContext, TArgs...>(_contextCtor, func, consumerCount, bufferSize);
        }

        template <typename TFn, typename = std::void_t<decltype(&std::remove_cvref_t<TFn>::template operator()<TContext>)>>
        auto schedule(TFn &&fn, size_t consumerCount = 0, size_t bufferSize = 0) {
            static_assert(std::is_empty_v<std::remove_cvref_t<TFn>>);

            return schedule(Specialize(std::forward<TFn>(fn)), consumerCount, bufferSize);
        }

      private:
        template <typename T>
        struct spec_helper;

        template <typename TClass, typename TContextArg, typename... TArgs>
        struct spec_helper<void (TClass::*)(TContextArg, TArgs...) const> {
            template <typename TFn>
            static auto GetSpecialized() {
                constexpr void (*ptr)(TContext &, TArgs && ...) = [](TContext &context, TArgs &&...args) {
                    static_cast<TFn *>(nullptr)->operator()(context, args...);
                };
                return ptr;
            }
        };

        template <typename TFn>
        static auto Specialize(const TFn &) {
            using func_t = std::remove_cvref_t<TFn>;
            using fptr_t = decltype(&func_t::template operator()<TContext &>);

            return spec_helper<fptr_t>::template GetSpecialized<func_t>();
        }
    };

    template <typename TCtor>
    parallel_context(TCtor &&) -> parallel_context<std::remove_cvref_t<std::invoke_result_t<TCtor>>>;

    template <typename TContext, typename... TJobArgs>
    class parallel_job : private parallel_traits<TContext> {
        friend class parallel_context<TContext>;

        using typename parallel_traits<TContext>::context_ctor_t;

        enum class task_state : int { empty,
                                      ready,
                                      occupied };

        using task_t       = std::tuple<TJobArgs...>;
        using task_value_t = std::tuple<std::remove_cvref_t<TJobArgs>...>;
        using task_ref_t   = std::tuple<std::add_lvalue_reference<TJobArgs>...>;
        using job_t        = void (*)(TContext &, TJobArgs...);
        using task_state_t = std::atomic<task_state>;

        class consumer {
            std::thread _thread;

            parallel_job *_schedule;

            TContext _context;

            job_t _job;

            size_t _cursor;

            task_value_t _task;

          public:
            consumer(parallel_job *schedule, context_ctor_t &contextCtor, const job_t &job)
                : _thread(consume, this), _schedule(schedule), _context(contextCtor()), _job(job), _cursor(0) {
                _thread.detach();
            }

            consumer(const consumer &) = delete;
            consumer(consumer &&)      = delete;

            static void consume(consumer *consumer) {
                parallel_job *const schedule = consumer->_schedule;

                while (!schedule->_exitFlag) {
                    size_t &cursor     = consumer->_cursor;
                    task_value_t &task = consumer->_task;
                    while (!schedule->tryRetrieveTask(cursor, &task))
                        schedule->incrementConsumerCursor(&cursor);

                    std::apply(consumer->_job, std::tuple_cat(std::forward_as_tuple(consumer->_context), task));
                }

                schedule->notifyExit();
            }
        };

        std::shared_ptr<consumer[]> _consumers;

        size_t _consumerCount;

        std::shared_ptr<task_value_t[]> _tasks;

        std::shared_ptr<task_state_t[]> _states;

        size_t _bufferSize;

        std::atomic<size_t> _producerCursor;

        std::atomic<int> _exitFlag;

        template <typename TJob>
        parallel_job(context_ctor_t &contextCtor, TJob &&job, size_t consumerCount, size_t bufferSize)
            : _consumers(context::AllocateSharedArray<consumer>(consumerCount)), _consumerCount(consumerCount),
              _tasks(context::AllocateSharedArray<task_value_t>(bufferSize)),
              _states(context::AllocateSharedArray<task_state_t>(bufferSize)),
              _bufferSize(bufferSize), _producerCursor(0), _exitFlag(0) {
            for (size_t i = 0; i < _consumerCount; i++)
                context::Construct(&_consumers[i], this, contextCtor, std::forward<TJob>(job));
        }

      public:
        parallel_job(const parallel_job &) = delete;
        parallel_job(parallel_job &&)      = delete;

        ~parallel_job() {
            finish();
            context::Destroy(_consumers.get(), _consumerCount);
        }

        template <typename... TArgs>
        void issue(TArgs &&...args) {
            task_state_t &state = _states[_producerCursor];

            state.wait(task_state::ready);
            state.wait(task_state::occupied);

            new (&_tasks[_producerCursor]) task_t(std::forward<TArgs>(args)...);
            state = task_state::ready;

            incrementProducerCursor();
        }

        void finish() {
            if (_exitFlag == -1)
                return;

            _exitFlag = _consumerCount;
            for (int i = _consumerCount; i > 0; i--)
                _exitFlag.wait(i);
            _exitFlag = -1;
        }

      private:
        size_t incrementCursor(size_t cursor, size_t n) {
            igiassert(cursor + n < _bufferSize * 2);

            cursor += n;
            return cursor < _bufferSize ? cursor : cursor - _bufferSize;
        }

        template <typename T>
        static void notifyProducer(std::atomic<T> &atomic) {
            atomic.notify_one();
        }

        bool tryRetrieveTask(size_t index, task_value_t *task) {
            task_state expected = task_state::ready;
            task_state_t &state = _states[index];
            if (state.compare_exchange_strong(expected, task_state::occupied)) {
                new (task) task_value_t(std::move(_tasks[index]));
                state = task_state::empty;
                notifyProducer(state);
                return true;
            }
            return false;
        }

        void incrementProducerCursor() {
            _producerCursor = incrementCursor(_producerCursor, 1);
            _producerCursor.notify_one();
        }

        void incrementConsumerCursor(size_t *cursor) {
            *cursor = incrementCursor(*cursor, 1);
            _producerCursor.wait(*cursor);
        }

        void notifyExit() {
            _exitFlag.fetch_sub(1);
            notifyProducer(_exitFlag);
        }
    };
}  // namespace igi
