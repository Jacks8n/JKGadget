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
            T col = (_buf[_index] * 255_col).clamp(0_col, 255_col);
            return pngparvus::pixel_rgb(static_cast<uint8_t>(col.r),
                                        static_cast<uint8_t>(col.g), static_cast<uint8_t>(col.b));
        }

        texture_color255_iterator &operator++() {
            _index++;
            return *this;
        }

        bool operator!=(const texture_color255_iterator &o) const {
            return _buf != o._buf;
        }
    };

    // TODO: support for optimal layout beyond the linear one
    template <typename T>
    class texture : public pngparvus::IPNG<texture_color255_iterator<T>> {
      public:
        class iterator : std::iterator_traits<T *> {
            friend class texture;

            std::shared_ptr<T[]> _buf;

            size_t _index;

            iterator(const std::shared_ptr<T[]> &buf, size_t index)
                : _buf(buf), _index(index) { }

          public:
            T &operator*() {
                return _buf[_index];
            }

            const T &operator*() const {
                return _buf[_index];
            }

            bool operator!=(const iterator &it) const {
                return it._index != _index || it._buf != _buf;
            }

            iterator &operator++() {
                _index++;
                return *this;
            }
        };

        using const_iterator = const iterator;

        META_BE(texture, rflite::func_a([](const serializer_t &ser) {
                    size_t w = serialization::Deserialize<size_t>(ser["width"]);
                    size_t h = serialization::Deserialize<size_t>(ser["height"]);
                    return rflite::meta_helper::any_ins<texture>(w, h);
                }))

        texture(const texture &o) = default;
        texture(texture &&o)      = default;

        texture(size_t w, size_t h)
            : _buf(AllocBuffer(w, h)), _w(w), _h(h) { }

        texture &operator=(const texture &) = delete;
        texture &operator=(texture &&) = delete;

        ~texture() = default;

        size_t getWidth() const override {
            return _w;
        }

        size_t getHeight() const override {
            return _h;
        }

        texture_color255_iterator<T> getPixels() const override {
            return texture_color255_iterator<T>(_buf);
        }

        iterator at(size_t u, size_t v) {
            assertInRange(u, v);
            return iterator(_buf, uvToIndex(u, v));
        }

        const_iterator at(size_t u, size_t v) const {
            assertInRange(u, v);
            return const_iterator(_buf, uvToIndex(u, v));
        }

        T &get(size_t u, size_t v) {
            return *at(u, v);
        }

        const T &get(size_t u, size_t v) const {
            return *at(u, v);
        }

        void clear(const T &col) {
            for (size_t j = 0; j < _h; j++)
                for (size_t i = 0; i < _w; i++)
                    get(i, j) = col;
        }

      private:
        std::shared_ptr<T[]> _buf;
        size_t _w, _h;

        static std::shared_ptr<T[]> AllocBuffer(size_t w, size_t h) {
            return std::allocate_shared<T[]>(context::GetTypedAllocator<T>(), w * h);
        }

        void assertInRange(size_t u, size_t v) const {
            assert(u < _w);
            assert(v < _h);
        }

        size_t uvToIndex(size_t u, size_t v) const {
            return v * _w + u;
        }
    };

    using texture_rgb = texture<color3>;
}  // namespace igi
