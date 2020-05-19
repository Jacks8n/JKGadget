#pragma once

#include <cmath>
#include "igimath/matrix.h"

namespace igi {
    template <typename T, size_t N>
    class matrix<T, N, 1> : public matrix_base<T, N, 1> {
        using matrix_base<T, N, 1>::matrix_base;

      public:
        constexpr matrix(const matrix<T, N - 1, 1> &v, const T &c)
            : matrix_base<T, N, 1>([&](size_t i, size_t j) constexpr { return i < N - 1 ? v[i] : c; }) { }

        explicit constexpr matrix(const matrix<T, N + 1, 1> &v)
            : matrix_base<T, N, 1>([&](size_t i, size_t j) constexpr { return v[i]; }) { }

        constexpr T magnitudeSqr() const {
            return Dot(*this, *this);
        }

        T magnitude() const {
            return static_cast<T>(sqrt(magnitudeSqr()));
        }

        matrix normalized() const {
            return *this * (static_cast<T>(1) / magnitude());
        }

        template <typename... Is>
        constexpr matrix permute(Is &&... is) const {
            return matrix(operator[](std::forward<Is>(is))...);
        }

        T &operator[](size_t i) {
            return matrix_base<T, N, 1>::get(i, 0);
        }

        constexpr const T &operator[](size_t i) const {
            return matrix_base<T, N, 1>::get(i, 0);
        }
    };

    template <typename T0, typename... Ts>
    constexpr auto igivec(T0 &&t0, Ts &&... ts) {
        return matrix<std::remove_reference_t<T0>, sizeof...(Ts) + 1, 1>(
            std::forward<T0>(t0), std::forward<Ts>(ts)...);
    }

    template <typename T, size_t N>
    using vec = matrix<T, N, 1>;

    template <typename T>
    using vec2 = vec<T, 2>;
    template <typename T>
    using vec3 = vec<T, 3>;
    template <typename T>
    using vec4 = vec<T, 4>;

    using vec2i = vec2<int>;
    using vec3i = vec3<int>;
    using vec4i = vec4<int>;

    using vec2f = vec2<single>;
    using vec3f = vec3<single>;
    using vec4f = vec4<single>;

    template <typename T, size_t N>
    constexpr T Dot(const matrix_base<T, N, 1> &l, const matrix_base<T, N, 1> &r) {
        return l.transMul(r);
    }

    template <typename T>
    constexpr vec3<T> Cross(const vec3<T> &l, const vec3<T> &r) {
        return vec3<T>(l[1] * r[2] - l[2] * r[1],
                       l[2] * r[0] - l[0] * r[2],
                       l[0] * r[1] - l[1] * r[0]);
    }

    template <typename T>
    constexpr vec3<T> Reflect(const vec3<T> i, const vec3<T> n) {
        return n * -2_sg * Dot(i, n) + i;
    }

    template <typename T>
    constexpr vec3<T> MakeReversedOrient(const vec3<T> &expected, const vec3<T> &v) {
        return IsPoscf(Dot(expected, v)) ? -v : v;
    }
}  // namespace igi
