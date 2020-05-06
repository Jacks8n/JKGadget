#pragma once

#include <memory_resource>
#include <mutex>

namespace igi {
    template <typename T>
    class circular_list {
      public:
        class iterator {
            friend class circular_list;

            T *_current, *const _buflo, *const _bufhi;

            iterator(T *const current, T *const buflo, T *const bufhi)
                : _current(current), _buflo(buflo), _bufhi(bufhi) { }

          public:
            T &operator*() {
                return *_current;
            }

            iterator &operator++() {
                if (++_current == _bufhi)
                    _current = _buflo;
                return *this;
            }

            bool operator!=(const iterator &o) const {
                return _current != o._current;
            }
        };

        using allocator_type = std::pmr::polymorphic_allocator<T>;

        circular_list(size_t size, const allocator_type &alloc)
            : _buflo(static_cast<T *>(alloc.resource()->allocate((size + 1) * sizeof(T), alignof(T)))),
              _bufhi(_buflo + size + 1), _begin(_buflo), _end(_buflo), _alloc(alloc) { }
        circular_list(const circular_list &o, const allocator_type &alloc)
            : _buflo(static_cast<T *>(alloc.resource()->allocate(o.capacity() + 1, alignof(T))), _alloc(alloc)),
              _bufhi(_buflo + o.capacity() + 1), _begin(_buflo + o.getBeginIndex()), _end(_buflo + o.getEndIndex()) {
        }
        circular_list(circular_list &&o, const allocator_type &alloc)
            : _buflo(o._buflo), _begin(o._begin), _end(o._end), _bufhi(o._bufhi), _alloc(alloc) {
            o._buflo = o._bufhi = o._begin = o._end = nullptr;
        }

        ~circular_list() {
            for (T &e : *this)
                e.~T();
            _alloc.deallocate(_buflo, _bufhi - _buflo);
        }

        allocator_type get_allocator() const noexcept {
            return _alloc;
        }

        size_t capacity() const {
            return _bufhi - _buflo - 1;
        }

        bool isEmpty() const {
            return _end == _begin;
        }

        bool isFull() const {
            return _end == _begin - 1 || (_end == _bufhi && _begin == _buflo);
        }

        iterator begin() const {
            return iterator(_begin, _buflo, _bufhi);
        }

        iterator end() const {
            return iterator(_end, nullptr, nullptr);
        }

        void push_back(const T &e) {
            emplace_back(e);
        }

        void push_back(T &&e) {
            emplace_back(std::move(e));
        }

        template <typename... Args>
        void emplace_back(Args &&... args) {
            assertNotFull();
            if (++_end == _bufhi)
                _end = _buflo;
            new (_end) T(std::forward<Args>(args)...);
        }

        T &&pop_back() {
            assertNotEmpty();
            T &&e = std::move(*_end);
            _end  = (_end == _buflo ? _bufhi : _end) - 1;
            return e;
        }

        T &&pop_front() {
            assertNotEmpty();
            T &e   = *_begin;
            _begin = (_begin == _bufhi ? _buflo : _begin) + 1;
            return std::move(e);
        }

      private:
        T *const _buflo, *const _bufhi;
        T *_begin, *_end;
        allocator_type _alloc;

        size_t getBeginIndex() const {
            return _begin - _buflo;
        }

        size_t getEndIndex() const {
            return _end - _buflo;
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
