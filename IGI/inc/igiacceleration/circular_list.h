#pragma once

#include <algorithm>
#include <memory_resource>
#include "igimath/mathutil.h"

namespace igi {
    template <typename T>
    class circular_list {
        template <typename U>
        class generic_iterator {
            friend class circular_list;

          public:
            using iterator_category = std::forward_iterator_tag;
            using value_type        = std::remove_const_t<U>;
            using difference_type   = std::ptrdiff_t;
            using pointer           = U *;
            using reference         = U &;

          private:
            U *_current, *const _buflo, *const _bufhi;

            generic_iterator(U *const current, U *const buflo, U *const bufhi)
                : _current(current), _buflo(buflo), _bufhi(bufhi) { }

          public:
            generic_iterator &operator=(const generic_iterator &o) {
                new (this) generic_iterator(o);
                return *this;
            }

            U &operator*() {
                return *_current;
            }

            generic_iterator &operator++() {
                if (++_current == _bufhi)
                    _current = _buflo;
                return *this;
            }

            bool operator!=(const generic_iterator &o) const {
                return _current != o._current;
            }

            std::ptrdiff_t operator-(const generic_iterator &r) const {
                return _current - r._current;
            }
        };

      public:
        using value_type      = T;
        using reference       = T &;
        using const_reference = const T &;
        using size_type       = size_t;

        using allocator_type = std::pmr::polymorphic_allocator<T>;
        using iterator       = generic_iterator<T>;
        using const_iterator = generic_iterator<const T>;

        circular_list() = default;
        circular_list(const circular_list &o) : circular_list(o, o.get_allocator()) { }
        circular_list(circular_list &&o) = default;

        circular_list(const allocator_type &alloc)
            : circular_list(8, alloc) { }

        circular_list(size_t size, const allocator_type &alloc)
            : _alloc(alloc), _buflo(_alloc.allocate(size + 1)),
              _bufhi(_buflo + size + 1), _begin(_buflo), _end(_bufhi) { }

        circular_list(const circular_list &o, const allocator_type &alloc)
            : _alloc(alloc), _buflo(alloc.allocate(o.getBufSize())),
              _bufhi(_buflo + o.getBufSize()), _begin(_buflo),
              _end(_buflo + o.size()) { }

        ~circular_list() {
            _alloc.deallocate(_buflo, getBufSize());
        }

        allocator_type get_allocator() const noexcept {
            return _alloc;
        }

        size_t capacity() const {
            return getBufSize() - 1;
        }

        size_t size() const {
            return _end >= _begin ? _end - _begin : getBufSize() - (_begin - _end);
        }

        void reserve(size_t n) {
            if (n <= getEmptySpace())
                return;

            const size_t cap = capacity(), sz = size();
            const size_t newcap = FloorExp2((n + sz - 1) / cap + 1) * cap;
            assert(newcap >= n + sz);

            circular_list tmp(newcap, get_allocator());
            tmp.resize(n + sz);
            std::move(begin(), end(), tmp.begin());

            this->~circular_list();
            new (this) circular_list(std::move(tmp));
        }

        void resize(size_t n) {
            const size_t sz = size();

            if (n > sz)
                reserve(n - sz);
            _end = _begin + n < _bufhi ? _begin + n : _end - (sz - n);
        }

        bool empty() const {
            return _end == _begin;
        }

        bool full() const {
            return size() == capacity();
        }

        iterator begin() {
            return iterator(_begin, _buflo, _bufhi);
        }

        iterator end() {
            return iterator(_end, _buflo, _bufhi);
        }

        const_iterator begin() const {
            return const_iterator(_begin, _buflo, _bufhi);
        }

        const_iterator end() const {
            return const_iterator(_end, _buflo, _bufhi);
        }

        T &front() {
            assertNotEmpty();
            return *_begin;
        }

        const T &front() const {
            assertNotEmpty();
            return *_begin;
        }

        T &back() {
            assertNotEmpty();
            return (_end > _buflo ? _end : _bufhi)[-1];
        }

        const T &back() const {
            assertNotEmpty();
            return (_end > _buflo ? _end : _bufhi)[-1];
        }

        void push_back(const T &e) {
            emplace_back(e);
        }

        void push_back(T &&e) {
            emplace_back(std::move(e));
        }

        template <typename... Args>
        void emplace_back(Args &&... args) {
            reserve(1);

            new (_end) T(std::forward<Args>(args)...);
            _end = _end + 1 == _bufhi ? _buflo : _end + 1;
        }

        void pop_back() {
            assertNotEmpty();
            _end = (_end == _buflo ? _bufhi : _end) - 1;
        }

        void pop_front() {
            assertNotEmpty();
            _begin = _begin == _bufhi - 1 ? _buflo : _begin + 1;
        }

        void clear() {
            _begin = _end = _buflo;
        }

        T &operator[](size_t i) {
            assert(i < size());
            return *ptrAt(i);
        }

        const T &operator[](size_t i) const {
            assert(i < size());
            return *ptrAt(i);
        }

      private:
        allocator_type _alloc;
        T *const _buflo, *const _bufhi;
        T *_begin, *_end;

        size_t getBufSize() const {
            return _bufhi - _buflo;
        }

        size_t getEmptySpace() const {
            return capacity() - size();
        }

        T *ptrAt(size_t i) {
            return _begin + i < _bufhi ? _begin + i : _buflo + (i - (_bufhi - _begin));
        }

        const T *ptrAt(size_t i) const {
            return _begin + i < _bufhi ? _begin + i : _buflo + (i - (_bufhi - _begin));
        }

        void assertNotEmpty() {
#if _DEBUG
            if (_begin == _end)
                throw;
#endif
        }

        void assertNotFull() {
#if _DEBUG
            if (isFull())
                throw;
#endif
        }
    };
}  // namespace igi
