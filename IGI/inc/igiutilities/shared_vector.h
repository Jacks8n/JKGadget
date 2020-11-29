#pragma once

#include <memory_resource>
#include "igiassert.h"

namespace igi {
    template <typename T>
    class shared_vector {
      public:
        using allocator_type = std::pmr::polymorphic_allocator<T>;

      private:
        allocator_type _alloc;

        std::shared_ptr<T[]> _buf;

        T *_end;

        const size_t _cap;

      public:
        using iterator       = T *;
        using const_iterator = const T *;

        shared_vector(size_t n, const allocator_type &alloc)
            : _alloc(alloc),
              _buf(_alloc.allocate(n), [=](T *p) {
                  _alloc.deallocate(p, n);
              }),
              _end(_buf.get() + n), _cap(n) { }

        template <typename U>
        shared_vector(std::initializer_list<U> il, const allocator_type &alloc)
            : shared_vector(il.size(), alloc) {
            for (size_t i = 0; i < il.size(); i++)
                alloc.construct(_buf.get() + i, il[i]);
        }

        template <typename... Ts>
        shared_vector(const allocator_type &alloc, Ts &&...ts)
            : shared_vector(sizeof...(Ts), alloc) {
            iterator lo = begin();
            ((new (&*lo) T(std::forward<Ts>(ts)), ++lo), ...);
        }

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
            size_t s = end() - begin();
            return s;
        }

        template <typename... Ts>
        iterator emplace_back(Ts &&...ts) {
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

        template <typename TD>
        void reset(TD &&deleter) {
            _buf.reset(_buf.get(), deleter);
        }

        const std::shared_ptr<T[]> &as_shared_ptr() const & {
            return _buf;
        }

        std::shared_ptr<T[]> &&as_shared_ptr() && {
            _end = nullptr;
            return std::move(_buf);
        }

        operator const std::shared_ptr<T[]> &() const & {
            return as_shared_ptr();
        }

        operator std::shared_ptr<T[]> &&() && {
            return as_shared_ptr();
        }

      private:
        void assert_space_free() const {
            igiassert(size() < capacity());
        }

        void assert_in_range(size_t i) const {
            igiassert(i < size());
        }
    };
}  // namespace igi
