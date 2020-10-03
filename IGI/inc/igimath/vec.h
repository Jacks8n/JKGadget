#pragma once

#include <cmath>
#include "igimath/matrix.h"
#include "igiutilities/serialize.h"

namespace igi {
    template <typename T, size_t N>
    class matrix<T, N, 1> : public matrix_base<T, N, 1> {
        using matrix_base<T, N, 1>::matrix_base;

      public:
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

        constexpr T magnitudeSqr() const {
            return Dot(*this, *this);
        }

        constexpr T magnitude() const {
            return static_cast<T>(SqrtConstexpr(magnitudeSqr()));
        }

        constexpr matrix normalized() const {
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
    template <size_t N>
    using vecf = matrix<single, N, 1>;

    template <typename T>
    using vec2 = vec<T, 2>;
    template <typename T>
    using vec3 = vec<T, 3>;
    template <typename T>
    using vec4 = vec<T, 4>;

    using vec2i = vec2<int>;
    using vec3i = vec3<int>;
    using vec4i = vec4<int>;

    using vec2f = vecf<2>;
    using vec3f = vecf<3>;
    using vec4f = vecf<4>;

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
