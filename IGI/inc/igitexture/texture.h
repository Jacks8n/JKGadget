#pragma once

#include <memory>
#include <utility>
#include "igitexture/color.h"
#include "png.h"

namespace igi {
    template<typename T>
    class texture_color255_encoder {};

    template<typename T>
    class texture : public pngparvus::IPNG<texture_color255_encoder<T>, color255<color_n_channel_v<T>>> {
        std::shared_ptr<T[]> _buf;

        size_t _w, _h;

    public:
        texture(const texture&) = default;
        texture(texture&&) = default;
        texture(size_t w, size_t h) : _buf(new T[w * h]), _w(w), _h(h) {}

        texture& operator=(const texture&) = default;
        texture& operator=(texture&&) = default;

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

        T& get(size_t i, size_t j) {
            return _buf[i * _w + j];
        }

        const T& get(size_t i, size_t j) const {
            return _buf[i * _w + j];
        }
    };

    using texture_rgb = texture<color_rgb>;
    using texture_rgba = texture<color_rgba>;

    template<size_t N>
    class texture_color255_encoder<color<N>> {
        std::shared_ptr<color<N>[]> _buf;

        size_t _index = 0;

    public:
        texture_color255_encoder() = delete;
        texture_color255_encoder(const texture_color255_encoder&) = default;
        texture_color255_encoder(texture_color255_encoder&&) = default;
        texture_color255_encoder(std::shared_ptr<color<N>[]> buf) : _buf(buf) {}

        texture_color255_encoder& operator=(const texture_color255_encoder&) = default;
        texture_color255_encoder& operator=(texture_color255_encoder&&) = default;

        ~texture_color255_encoder() = default;

        color255<N> operator*() const {
            return ToColor255(_buf[_index]);
        }

        texture_color255_encoder& operator++() {
            _index++;
            return *this;
        }

        bool operator==(const texture_color255_encoder& o) const {
            return _buf == o._buf;
        }
    };
}
