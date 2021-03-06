﻿#pragma once

#include <cmath>
#include "igimath/matrix.h"
#include "igiutilities/serialize.h"

namespace igi {
    template <typename T, size_t N>
    requires(N > 1) class matrix<T, N, 1> : public matrix_base<T, N, 1> {
      public:
        using typename matrix_base<T, N, 1>::ref_element_t;
        using typename matrix_base<T, N, 1>::cref_element_t;

        using matrix_base<T, N, 1>::matrix_base;

        using matrix_base<T, N, 1>::begin;
        using matrix_base<T, N, 1>::end;
        using matrix_base<T, N, 1>::row;
        using matrix_base<T, N, 1>::col;

        META_BE(matrix, rflite::func_a([](const serializer_t &ser) {
                    if (!ser.IsArray() || ser.Size() != N)
                        throw;

                    rflite::any_defer<matrix> res = rflite::meta_helper::any_ins<matrix>();

                    size_t n = 0;
                    for (auto i = ser.Begin(); i != ser.End(); ++i)
                        new (&res->operator[](n++)) T(serialization::Deserialize<T>(*i));
                    return res;
                }))

        constexpr matrix(const matrix<T, N - 1, 1> &v, const T &c)
            : matrix_base<T, N, 1>([&](size_t i, size_t j) constexpr { return i < N - 1 ? v[i] : c; }) { }

        explicit constexpr matrix(const matrix<T, N + 1, 1> &v)
            : matrix_base<T, N, 1>([&](size_t i, size_t j) constexpr { return v[i]; }) { }

        constexpr T l1norm() const {
            return std::accumulate(begin(), end(), static_cast<T>(0),
                                   [](const T &l, const T &r) { return l + Abs(r); });
        }

        constexpr T magnitudeSqr() const {
            return Dot(*this, *this);
        }

        constexpr T magnitude() const {
            return static_cast<T>(Sqrt(magnitudeSqr()));
        }

        constexpr matrix normalized() const {
            return *this * (static_cast<T>(1) / magnitude());
        }

        constexpr bool isNormalized() const {
            using precise_t = error_single<T>;

            matrix<precise_t, 3, 1> e(*this);
            return e.magnitude() == static_cast<precise_t>(1);
        }

        template <typename... Is>
        constexpr matrix permute(Is &&...is) const {
            return matrix(operator[](std::forward<Is>(is))...);
        }

        decltype(auto) operator[](size_t i) {
            igiassert(i < N);
            return matrix_base<T, N, 1>::get(i, 0);
        }

        constexpr decltype(auto) operator[](size_t i) const {
            igiassert(i < N);
            return matrix_base<T, N, 1>::get(i, 0);
        }

        template <size_t Nsub>
        requires(Nsub < N) constexpr operator matrix<ref_element_t, Nsub, 1>() {
            return row(std::make_index_sequence<Nsub>());
        }

        template <size_t Nsub>
        requires(Nsub < N) constexpr operator matrix<cref_element_t, Nsub, 1>() const {
            return row(std::make_index_sequence<Nsub>());
        }
    };

    template <typename T0, typename... Ts>
    constexpr auto igivec(T0 &&t0, Ts &&...ts) {
        return matrix<std::remove_cvref_t<T0>, sizeof...(Ts) + 1, 1>(
            std::forward<T0>(t0), std::forward<Ts>(ts)...);
    }

    template <typename T, size_t N>
    using vec = matrix<T, N, 1>;
    template <size_t N>
    using veci = vec<int, N>;
    template <size_t N>
    using vecu = vec<unsigned, N>;
    template <size_t N>
    using vecf = vec<single, N>;
    template <size_t N>
    using vecef = vec<esingle, N>;

    template <typename T>
    using vec2 = vec<T, 2>;
    template <typename T>
    using vec3 = vec<T, 3>;
    template <typename T>
    using vec4 = vec<T, 4>;

    using vec2i = veci<2>;
    using vec3i = veci<3>;
    using vec4i = veci<4>;

    using vec2u = vecu<2>;
    using vec3u = vecu<3>;
    using vec4u = vecu<4>;

    using vec2f = vecf<2>;
    using vec3f = vecf<3>;
    using vec4f = vecf<4>;

    using vec2ef = vecef<2>;
    using vec3ef = vecef<3>;
    using vec4ef = vecef<4>;

    template <typename T, size_t N>
    constexpr T Dot(const vec<T, N> &l, const vec<T, N> &r) {
        return l.transMul(r);
    }

    template <typename T, size_t N>
    constexpr vec<T, N> Scale(const vec<T, N> &l, const vec<T, N> &r) {
        return vec<T, N>([&](size_t i, size_t) { return l[i] * r[i]; });
    }

    template <typename T>
    constexpr T Cross(const vec2<T> &l, const vec2<T> &r) {
        return l[0] * r[1] - l[1] * r[0];
    }

    template <typename T>
    constexpr vec3<T> Cross(const vec3<T> &l, const vec3<T> &r) {
        return vec3<T>(l[1] * r[2] - l[2] * r[1],
                       l[2] * r[0] - l[0] * r[2],
                       l[0] * r[1] - l[1] * r[0]);
    }

    template <typename T>
    constexpr vec3<T> Reflect(const vec3<T> i, const vec3<T> n) {
        return n * Dot(i, n) * -2 + i;
    }

    template <typename T>
    constexpr vec3<T> MakeReversedOrient(const vec3<T> &expected, const vec3<T> &v) {
        return Dot(expected, v) > 0 ? -v : v;
    }

    /// @return returns [up, right]
    template <typename T>
    constexpr std::pair<vec3<T>, vec3<T>> LookAt(const vec3<T> &forward, const vec3<T> &up) {
        vec3<T> right = Cross(forward, up).normalized();
        return std::make_pair(Cross(right, forward).normalized(), right);
    }

    /// @param forward is expected to be normalized
    template <typename T>
    constexpr mat4<T> LookAtMat(const vec3<T> &forward, const vec3<T> &up) {
        auto [eyeUp, eyeRight] = LookAt(forward, up);
        return mat4<T>(vec4<T>(eyeRight, 0), vec4<T>(eyeUp, 0), vec4<T>(forward, 0), vec4<T>(0, 0, 0, 1));
    }
}  // namespace igi
