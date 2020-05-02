#pragma once

#include <tuple>
#include "igimath/mathutil.h"
#include "igimath/vec.h"

namespace igi {
    using color_channel_t = float;

    template <size_t N>
    using color = vec<N, color_channel_t>;

    using color_rgb  = color<3>;
    using color_rgba = color<4>;

    template <size_t N>
    using color255 = vec<N, char>;

    using color255_rgb  = color255<3>;
    using color255_rgba = color255<4>;

    template<typename T>
    struct color_n_channel {};

    template<size_t N>
    struct color_n_channel<color<N>> {
        static constexpr size_t value = N;
    };

    template<typename T>
    constexpr size_t color_n_channel_v = color_n_channel<T>::value;

    struct palette_rgb {
        using color_t = color_rgb;

        static constexpr color_t white = color_t::One(1.f);

        static constexpr color_t black = color_t::One(0.f);
    };

    template <size_t N>
    constexpr color<N> Saturate(const color<N> &col) {
        return _saturate_impl(col, std::make_index_sequence<N>());
    }

    template <size_t N>
    constexpr color<N> Mul(const color<N> &l, const color<N> &r) {
        return _mul_impl(l, r, std::make_index_sequence<N>());
    }

    template <size_t N>
    constexpr color_channel_t Brightness(const color<N> &col) {
        return (color_channel_t).299 * col[0] + (color_channel_t).587 * col[1] + (color_channel_t).114 * col[2];
    }

    template <size_t N>
    constexpr color<N> ToBaseColor(const color<N>& col) {
        return col * ((color_channel_t)1 / Brightness(col));
    }

    template <size_t N>
    constexpr color255<N> ToColor255(const color<N> &col) {
        return _toColor255_impl(col, std::make_index_sequence<N>());
    }

    template <size_t N, size_t... Is>
    constexpr color<N> _saturate_impl(const color<N> &col, std::index_sequence<Is...>) {
        return color<N>(Saturate(col[Is])...);
    }

    template <size_t N, size_t... Is>
    constexpr color<N> _mul_impl(const color<N> &l, const color<N> &r, std::index_sequence<Is...>) {
        return color<N>((l[Is] * r[Is])...);
    }

    template <size_t N, size_t... Is>
    constexpr color255<N> _toColor255_impl(const color<N> &col, std::index_sequence<Is...>) {
        return color255<N>(static_cast<char>(Clamp(0.f, 255.f, col[Is] * 255.f))...);
    }
}  // namespace igi
