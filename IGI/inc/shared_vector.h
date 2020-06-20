#pragma once

#include <assert.h>
#include <memory>

namespace igi {
    template <typename T>
    class shared_vector {
        std::shared_ptr<T[]> _buf;

        T *_end;

        const size_t _cap;

      public:
        using iterator       = T *;
        using const_iterator = const T *;

        template <typename TAlloc>
        shared_vector(size_t n, TAlloc &&alloc)
            : _buf(std::allocator_traits<TAlloc>::allocate(alloc, n)), _end(_buf.get() + n), _cap(n) { }
        template <typename TAlloc, typename TD>
        shared_vector(size_t n, TAlloc &&alloc, TD &&dtor)
            : _buf(std::allocator_traits<TAlloc>::allocate(alloc, n), std::forward<TD>(dtor)), _end(_buf.get() + n), _cap(n) { }

        iterator begin() {
            return _buf.get();
        }

        const_iterator begin() const {
            return _buf.get();
        }

        iterator end() {
            return _end;
        }

        const_iterator end() const {
            return _end;
        }

        size_t capacity() const {
            return _cap;
        }

        size_t size() const {
            return end() - begin();
        }

        template <typename... Ts>
        iterator emplace_back(Ts &&... ts) {
            assert_space_free();
            return new (_end++) T(std::forward<Ts>(ts)...);
        }

        void push_back(T &&val) {
            emplace_back(std::move(val));
        }

        void push_back(const T &val) {
            emplace_back(val);
        }

        T &operator[](size_t i) {
            assert_in_range(i);
            return _buf[i];
        }

        const T &operator[](size_t i) const {
            assert_in_range(i);
            return _buf[i];
        }

        template <typename... Ts>
        std::shared_ptr<T[]> as_shared_ptr(Ts &&... ts) & {
            return std::shared_ptr<T[]>(_buf, std::forward<Ts>(ts)...);
        }

        template <typename... Ts>
        std::shared_ptr<T[]> as_shared_ptr(Ts &&... ts) && {
            _cap = 0;
            _end = nullptr;
            return std::shared_ptr<T[]>(std::move(_buf), std::forward<Ts>(ts)...);
        }

        operator std::shared_ptr<T[]>() & {
            return _buf;
        }

        operator std::shared_ptr<T[]>() && {
            _cap = 0;
            _end = nullptr;
            return std::move(_buf);
        }

      private:
        void assert_space_free() const {
            assert(size() < capacity());
        }

        void assert_in_range(size_t i) const {
            assert(i < size());
        }
    };
}  // namespace igi