#pragma once

#include <tuple>
#include "igimath/mathutil.h"
#include "igimath/vec.h"

namespace igi {
    using color_channel_t = float;

    template <size_t N>
    using color = vec<color_channel_t, N>;

    using color_rgb  = color<3>;
    using color_rgba = color<4>;

    using colorI_channel_t = uint8_t;

    template <size_t N, typename T = colorI_channel_t>
    using colorI = vec<T, N>;

    using colorI_rgb  = colorI<3>;
    using colorI_rgba = colorI<4>;

    template <typename T>
    struct color_n_channel { };

    template <size_t N>
    struct color_n_channel<color<N>> {
        static constexpr size_t value = N;
    };

    template <typename T>
    constexpr size_t color_n_channel_v = color_n_channel<T>::value;

    constexpr color_channel_t operator""_col(unsigned long long val) {
        return static_cast<color_channel_t>(val);
    }

    constexpr color_channel_t operator""_col(long double val) {
        return static_cast<color_channel_t>(val);
    }

    constexpr colorI_channel_t operator""_coli(unsigned long long val) {
        return static_cast<colorI_channel_t>(val);
    }

    constexpr colorI_channel_t operator""_coli(long double val) {
        return static_cast<colorI_channel_t>(val);
    }

    struct palette_rgb {
        using color_t = color_rgb;

        static constexpr color_t white = color_t::One(1_col);

        static constexpr color_t black = color_t::One(0_col);

        static constexpr color_t red = color_t(1_col, 0_col, 0_col);

        static constexpr color_t green = color_t(0_col, 1_col, 0_col);

        static constexpr color_t blue = color_t(0_col, 0_col, 1_col);

        static constexpr color_t yellow = color_t(1_col, 1_col, 0_col);
    };

    namespace __igiimpl {
        template <size_t N, size_t... Is>
        constexpr color<N> _saturate_impl(const color<N> &col, std::index_sequence<Is...>) {
            return color<N>(Saturate(col[Is])...);
        }

        template <size_t N, size_t... Is>
        constexpr color<N> _mul_impl(const color<N> &l, const color<N> &r, std::index_sequence<Is...>) {
            return color<N>((l[Is] * r[Is])...);
        }

        template <size_t N, typename T, size_t... Is>
        constexpr colorI<N, T> _toColor255_impl(const color<N> &col, std::index_sequence<Is...>) {
            return colorI<N, T>(static_cast<T>(Clamp(0_col, 255_col, col[Is] * 255_col))...);
        }
    }  // namespace __igiimpl

    template <size_t N>
    constexpr color<N> Saturate(const color<N> &col) {
        return __igiimpl::_saturate_impl(col, std::make_index_sequence<N>());
    }

    template <size_t N>
    constexpr color<N> Mul(const color<N> &l, const color<N> &r) {
        return __igiimpl::_mul_impl(l, r, std::make_index_sequence<N>());
    }

    template <size_t N>
    constexpr color_channel_t Brightness(const color<N> &col) {
        return .299_col * col[0] + .587_col * col[1] + .114_col * col[2];
    }

    template <size_t N>
    constexpr color<N> ToBaseColor(const color<N> &col) {
        return col * (1_col / Brightness(col));
    }

    template <size_t N, typename T = colorI_channel_t>
    constexpr colorI<N> ToColor255(const color<N> &col) {
        return __igiimpl::_toColor255_impl<N, T>(col, std::make_index_sequence<N>());
    }
}  // namespace igi
