#pragma once

#include <memory_resource>

namespace igi {
    template <typename T>
    class circular_list {
      public:
        using allocator_type = std::pmr::polymorphic_allocator<T>;

        circular_list(size_t size, const allocator_type &alloc)
            : _buflo(alloc.resource()->allocate((size + 1) * sizeof(T), alignof(T))),
              _begin(0), _end(0), _alloc(alloc) {
            _bufhi = _buflo + size + 1;
        }
        circular_list(const circular_list &o, const allocator_type &alloc)
            : _buflo(alloc.resource()->allocate(o.capacity() + 1, alignof(T))), _alloc(alloc) {
            _bufhi = _buflo + o.capacity() + 1;
            _begin = _buflo + o.getBeginIndex();
            _end   = _buflo + o.getEndIndex();
        }
        circular_list(circular_list &&o, const allocator_type &alloc)
            : _buflo(o._buflo), _begin(o._begin), _end(o._end), _bufhi(o._bufhi), _alloc(alloc) {
            o._buflo = o._bufhi = o._begin = o._end = nullptr;
        }

        ~circular_list() = default;

        allocator_type get_allocator() const noexcept {
            return _alloc;
        }

        size_t capacity() const {
            return _bufhi - _buflo - 1;
        }

        size_t count() const {
            return _begin <= _end ? _end - _begin : _bufhi - _buflo - (_begin - _end);
        }

        bool isFull() const {
            return (_end == _bufhi && _begin == _buflo) || _end == _begin - 1;
        }

        void push_back(const T &e) {
            emplace_back(e);
        }

        void push_back(T &&e) {
            emplace_back(std::move(e));
        }

        template <typename... Args>
        void emplace_back(Args &&... args) {
            if (isFull())
                throw;
            new (_end++) T(std::forward<Args>(args)...);
        }

        T &&pop_back() {
            assertNotEmpty();
            T &&e = std::move(*_end);
            _end  = (_end == _buflo ? _bufhi : _end) - 1;
            return e;
        }

        T &&pop_front() {
            assertNotEmpty();
            T &&e  = std::move(*_begin);
            _begin = (_begin == _bufhi ? _buflo : _begin) + 1;
            return e;
        }

      private:
        T *const _buflo, *const _bufhi;
        T *_begin, *_end;
        const allocator_type &_alloc;

        size_t getBeginIndex() const {
            return _begin - _buflo;
        }

        size_t getEndIndex() const {
            return _end - _buflo;
        }

        void assertNotEmpty() {
            if (_begin == _end)
                throw;
        }
    };
}  // namespace igi