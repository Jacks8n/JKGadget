#pragma once

#include <memory_resource>
#include "igicontext.h"
#include "igitexture/color.h"
#include "png.h"

namespace igi {
    template <typename T>
    class texture;

    template <typename T>
    class texture_color255_iterator : public std::iterator_traits<pngparvus::pixel_rgb *> {
        friend class texture<T>;

        const std::shared_ptr<T[]> _buf;

        size_t _index;

        explicit texture_color255_iterator(const std::shared_ptr<T[]> &buf) : _buf(buf), _index(0) { }

      public:
        pngparvus::pixel_rgb operator*() const {
            const T col = (_buf[_index] * 255_col).clamp(0_col, 255_col);
            return pngparvus::pixel_rgb(static_cast<uint8_t>(col.r),
                                        static_cast<uint8_t>(col.g), static_cast<uint8_t>(col.b));
        }

        texture_color255_iterator &operator++() {
            ++_index;
            return *this;
        }

        bool operator!=(const texture_color255_iterator &o) const {
            return _buf != o._buf || _index != o._index;
        }
    };

    template <typename T>
    class texture : public pngparvus::IPNG<texture_color255_iterator<T>> {
      public:
        using coord_t = unsigned;
        using index_t = size_t;

        class iterator : std::iterator_traits<T *> {
            friend class texture;

            std::weak_ptr<T[]> _buf;

            size_t _index;

            igiward(_size, size_t);

            iterator(const std::shared_ptr<T[]> &buf, size_t index)
                : _buf(buf), _index(index) { }

          public:
            T &operator*() {
                std::shared_ptr<T[]> ptr = _buf.lock();
                igiassert(ptr);
                return ptr[_index];
            }

            const T &operator*() const {
                std::shared_ptr<T[]> ptr = _buf.lock();
                igiassert(ptr);
                return ptr[_index];
            }

            iterator &operator++() {
                ++_index;
                igiassert(_index <= _size);
                return *this;
            }

            bool operator!=(const iterator &it) const {
                return it._index != _index || it._buf.lock() != _buf.lock();
            }
        };

      private:
        std::shared_ptr<T[]> _buf;

        coord_t _w, _h;

      public:
        META_BE(texture, rflite::func_a([](const serializer_t &ser) {
                    size_t w = serialization::Deserialize<size_t>(ser["width"]);
                    size_t h = serialization::Deserialize<size_t>(ser["height"]);
                    return rflite::meta_helper::any_ins<texture>(w, h);
                }))

        texture(const texture &o) = default;
        texture(texture &&o)      = default;

        texture(size_t w, size_t h)
            : _buf(context::AllocateSharedArray<T>(w * h)), _w(w), _h(h) { }

        texture &operator=(const texture &) = delete;
        texture &operator=(texture &&) = delete;

        size_t getWidth() const override {
            return _w;
        }

        size_t getHeight() const override {
            return _h;
        }

        size_t getPixelCount() const {
            return _w * _h;
        }

        iterator begin() {
            iterator it(_buf, 0);
            igiward_set(it, _size, getPixelCount());

            return it;
        }

        iterator end() {
            const size_t pixel = getPixelCount();

            iterator it(_buf, pixel);
            igiward_set(it, _size, pixel);

            return it;
        }

        T &at(const coord_t &u, const coord_t &v) {
            assertInRange(u, v);
            return _buf[uvToIndex(u, v)];
        }

        const T &at(const coord_t &u, const coord_t &v) const {
            assertInRange(u, v);
            return _buf[uvToIndex(u, v)];
        }

        void clear(const T &col) {
            std::fill(begin(), end(), col);
        }

      private:
        void assertInRange(const coord_t &u, const coord_t &v) const {
            igiassert(0 <= u && u < _w);
            igiassert(0 <= v && v < _h);
        }

        index_t uvToIndex(const coord_t &u, const coord_t &v) const {
            return _w * v + u;
        }

        texture_color255_iterator<T> getPixels() const override {
            return texture_color255_iterator<T>(_buf);
        }
    };

    using texture_rgb = texture<color3>;
}  // namespace igi
