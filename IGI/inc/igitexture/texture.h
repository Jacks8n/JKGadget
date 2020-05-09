#pragma once

#include <memory>
#include <utility>
#include "igitexture/color.h"
#include "png.h"

namespace igi {
    template <typename T>
    class texture_color255_encoder { };

    template <typename T>
    class texture : public pngparvus::IPNG<texture_color255_encoder<T>, color255<color_n_channel_v<T>>> {
      public:
        using allocator_type = std::pmr::polymorphic_allocator<T>;

        texture(const texture &o, const allocator_type &alloc)
            : _alloc(alloc), _buf(allocBuffer(w, h, _alloc)), _w(o._w), _h(o._h) {
            std::copy(&o.get(0, 0), &o.get(_w, _h), &get(0, 0));
        }
        texture(texture &&o, const allocator_type &alloc)
            : _alloc(alloc), _buf(std::move(o._buf)), _w(o._w), _h(o._h) { }
        texture(size_t w, size_t h, const allocator_type &alloc)
            : _alloc(alloc), _buf(allocBuffer(w, h, _alloc)), _w(w), _h(h) { }

        texture &operator=(const texture &) = delete;
        texture &operator=(texture &&) = delete;

        ~texture() = default;

        size_t getWidth() const override {
            return _w;
        }

        size_t getHeight() const override {
            return _h;
        }

        texture_color255_encoder<T> getPixels() const override {
            return texture_color255_encoder<T>(_buf);
        }

        T &get(size_t u, size_t v) {
            return _buf[u + v * _w];
        }

        const T &get(size_t u, size_t v) const {
            return _buf[u + v * _w];
        }

        void clear(const T &col) {
            for (size_t j = 0; j < _h; j++)
                for (size_t i = 0; i < _w; i++)
                    get(i, j) = col;
        }

      private:
        allocator_type _alloc;
        std::shared_ptr<T[]> _buf;
        size_t _w, _h;

        static constexpr auto allocBuffer(size_t w, size_t h, allocator_type &alloc) {
            return std::shared_ptr<T[]>(
                alloc.allocate(w * h),
                [&](T *p) { p->~T(); alloc.deallocate(p, w * h); },
                alloc);
        }
    };

    using texture_rgb  = texture<color_rgb>;
    using texture_rgba = texture<color_rgba>;

    template <size_t N>
    class texture_color255_encoder<color<N>> {
        std::shared_ptr<color<N>[]> _buf;

        size_t _index = 0;

      public:
        texture_color255_encoder()                                 = delete;
        texture_color255_encoder(const texture_color255_encoder &) = default;
        texture_color255_encoder(texture_color255_encoder &&)      = default;
        explicit texture_color255_encoder(std::shared_ptr<color<N>[]> buf) : _buf(buf) { }

        texture_color255_encoder &operator=(const texture_color255_encoder &) = default;
        texture_color255_encoder &operator=(texture_color255_encoder &&) = default;

        ~texture_color255_encoder() = default;

        color255<N> operator*() const {
            return ToColor255(_buf[_index]);
        }

        texture_color255_encoder &operator++() {
            _index++;
            return *this;
        }

        bool operator==(const texture_color255_encoder &o) const {
            return _buf == o._buf;
        }
    };
}  // namespace igi
