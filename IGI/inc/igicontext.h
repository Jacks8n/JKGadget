#pragma once

#include <memory_resource>
#include "igiacceleration/mem_arena.h"

namespace igi {
    enum class allocate_usage : size_t { persistent,
                                         temp,
                                         max };

    class context {
      public:
        using allocator_t = std::pmr::polymorphic_allocator<char>;

        template <typename T>
        using allocator_generic_t = std::pmr::polymorphic_allocator<T>;

      private:
        struct allocator_wrapper {
            mem_arena MemoryArena;
            allocator_t Allocator;

            allocator_wrapper(allocator_t *externalAllocator) {
                if (externalAllocator)
                    new (&Allocator) allocator_t(*externalAllocator);
                else {
                    new (&MemoryArena) mem_arena();
                    new (&Allocator) allocator_t(&MemoryArena);
                }
            }
        };

      public:
        static inline allocator_t *ExternalAllocator = nullptr;

        static inline allocator_t *ExternalTempAllocator = nullptr;

        template <allocate_usage Usage = allocate_usage::persistent>
        static allocator_t &GetAllocator() {
            static allocator_wrapper Allocator { Usage == allocate_usage::persistent ? ExternalAllocator
                                                 : Usage == allocate_usage::temp     ? ExternalTempAllocator
                                                                                     : nullptr };

            return Allocator.Allocator;
        }

        template <typename T, allocate_usage Usage = allocate_usage::persistent>
        static allocator_generic_t<T> &GetTypedAllocator() {
            static allocator_generic_t<T> Allocator { GetAllocator<Usage>() };

            return Allocator;
        }

        template <typename T, allocate_usage Usage = allocate_usage::persistent>
        static decltype(auto) Allocate(size_t n = 1) {
            using allocator_t = allocator_generic_t<T>;

            return std::allocator_traits<allocator_t>::allocate(GetTypedAllocator<T, Usage>(), n);
        }

        template <typename T, allocate_usage Usage = allocate_usage::persistent, typename... TArgs>
        static decltype(auto) Construct(T *p, TArgs &&...args) {
            using allocator_t = allocator_generic_t<T>;

            return std::allocator_traits<allocator_t>::construct(GetTypedAllocator<T, Usage>(), p, std::forward<TArgs>(args)...);
        }

        template <typename T, allocate_usage Usage = allocate_usage::persistent, typename... TArgs>
        static decltype(auto) New(TArgs &&...args) {
            auto buf = Allocate<T, Usage>();
            Construct<T, Usage>(buf, std::forward<TArgs>(args)...);
            return buf;
        }

        template <typename T, allocate_usage Usage = allocate_usage::persistent, typename... TArgs>
        static decltype(auto) NewArray(size_t n, TArgs &&...args) {
            auto buf = Allocate<T, Usage>(n);
            for (size_t i = 0; i < n; i++)
                Construct<T, Usage>(&buf[i], std::forward<TArgs>(args)...);
            return buf;
        }

        template <allocate_usage Usage = allocate_usage::persistent, typename T>
        static void Destroy(T *p, size_t n = 1) {
            using allocator_t = allocator_generic_t<T>;

            for (size_t i = 0; i < n; i++)
                std::allocator_traits<allocator_t>::destroy(GetTypedAllocator<T, Usage>(), p++);
        }

        template <allocate_usage Usage = allocate_usage::persistent, typename T>
        static decltype(auto) Deallocate(T *p, size_t n = 1) {
            using allocator_t = allocator_generic_t<T>;

            return std::allocator_traits<allocator_t>::deallocate(GetTypedAllocator<T, Usage>(), p, n);
        }

        template <allocate_usage Usage = allocate_usage::persistent, typename T>
        static decltype(auto) Delete(T *p, size_t n = 1) {
            while (n--)
                Destroy(p + n);
            return Deallocate(p);
        }

        template <typename T, allocate_usage Usage = allocate_usage::persistent>
        static std::shared_ptr<T> AllocateShared() {
            return std::shared_ptr<T>(Allocate<T, Usage>(), [](T *ptr) { Deallocate<Usage>(ptr); });
        }

        template <typename T, allocate_usage Usage = allocate_usage::persistent>
        static std::shared_ptr<T[]> AllocateSharedArray(size_t n) {
            return std::shared_ptr<T[]>(Allocate<T, Usage>(n), [](T *ptr) { Deallocate<Usage>(ptr); });
        }
    };
}  // namespace igi