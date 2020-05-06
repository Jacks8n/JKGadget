#pragma once

#include <memory_resource>
#include <new>

namespace igi {
    class mem_arena : public std::pmr::memory_resource {
        class chunk {
            chunk *_prev;

            size_t _size, _align;

            void *_available;

            chunk(chunk *const prev, size_t size, size_t align, void *available) noexcept
                : _prev(prev), _size(size), _align(align), _available(available) { }

          public:
            static chunk *AllocChunk(size_t size, size_t align, chunk *prev) noexcept {
                size_t head = sizeof(chunk) < align ? align : sizeof(chunk);
                void *buf   = ::operator new(size + head, std::align_val_t(align), std::nothrow);
                return new (buf) chunk(prev, size, align, reinterpret_cast<char *>(buf) + head);
            }

            chunk *getPrev() noexcept { return _prev; }

            size_t getAlign() noexcept { return _align; }

            void *tryAlloc(size_t size, size_t align) noexcept {
                char *thisp = reinterpret_cast<char *>(this);
                char *alloc = reinterpret_cast<char *>(ceilAddr(_available, align));

                if (thisp + _size < alloc + size)
                    return nullptr;

                _available = alloc + size;
                return alloc;
            }

            void *alloc(size_t size) noexcept {
                void *alloc = _available;
                _available  = reinterpret_cast<char *>(_available) + size;
                return alloc;
            }
        };

        class chunk_list {
            chunk *_last = nullptr;

          public:
            chunk_list(size_t init_size, size_t align) noexcept {
                allocChunk(init_size, align);
            }

            chunk &getLast() noexcept { return *_last; }

            chunk &allocChunk(size_t size, size_t align) noexcept {
                return *(_last = chunk::AllocChunk(static_cast<size_t>(size * 1.36), align, _last));
            }

            void releaseLast() noexcept {
                chunk *prev = _last->getPrev();
                ::operator delete(_last, std::align_val_t(_last->getAlign()), std::nothrow);
                _last = prev;
            }

            bool isEmpty() noexcept { return _last == nullptr; }
        };

        chunk_list _chunks;

      public:
        mem_arena(size_t init_size, size_t align = alignof(void *)) noexcept
            : _chunks(init_size, align) { }
        mem_arena(const mem_arena &)     = delete;
        mem_arena(mem_arena &&) noexcept = default;

        ~mem_arena() { release(); }

        void release() noexcept {
            while (!_chunks.isEmpty())
                _chunks.releaseLast();
        }

      private:
        void *do_allocate(size_t _Bytes, size_t _Align) override {
            void *alloc = _chunks.getLast().tryAlloc(_Bytes, _Align);
            return alloc ? alloc : _chunks.allocChunk(_Bytes, _Align).alloc(_Bytes);
        }

        void do_deallocate(void *_Ptr, size_t _Bytes, size_t _Align) override { }

        bool do_is_equal(const memory_resource &_That) const noexcept override {
            return &_That == this;
        }

        static void *ceilAddr(void *add, size_t align) {
            uintptr_t a = reinterpret_cast<uintptr_t>(add);
            return reinterpret_cast<void *>((a - 1) / align * align + align);
        }
    };
}  // namespace igi
