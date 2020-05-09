#pragma once

#include <cmath>
#include "igimath/matrix.h"

namespace igi {
    template <size_t N, typename T>
    class matrix<N, 1, T> : public matrix_base<N, 1, T> {
        using matrix_base<N, 1, T>::matrix_base;

      public:
        matrix(const matrix<N - 1, 1, T> &v, T c) {
            for (size_t i = 0; i < N - 1; i++)
                operator[](i) = v[i];
            operator[](N - 1) = c;
        }

        using matrix_base<N, 1, T>::One;

        static constexpr matrix Infinity() {
            return One(SingleInfinity);
        }

        explicit matrix(const matrix<N + 1, 1, T> &v) {
            for (size_t i = 0; i < N; i++)
                (*this)[i] = v[i];
        }

        T magnitudeSqr() const {
            return Dot(*this, *this);
        }

        T magnitude() const {
            return static_cast<T>(sqrt(magnitudeSqr()));
        }

        matrix normalized() const {
            return *this * (static_cast<T>(1) / magnitude());
        }

        T &operator[](size_t i) {
            return matrix_base<N, 1, T>::get(i, 0);
        }

        const T &operator[](size_t i) const {
            return matrix_base<N, 1, T>::get(i, 0);
        }

        matrix &operator*=(T s) {
            *this = *this * s;
            return *this;
        }
    };

    template <size_t N, typename T>
    using vec = matrix<N, 1, T>;

    template <typename T>
    using vec2 = vec<2, T>;
    template <typename T>
    using vec3 = vec<3, T>;
    template <typename T>
    using vec4 = vec<4, T>;

    using vec2i = vec2<int>;
    using vec3i = vec3<int>;
    using vec4i = vec4<int>;

    using vec2f = vec2<single>;
    using vec3f = vec3<single>;
    using vec4f = vec4<single>;

    template <size_t N, typename T>
    inline T Dot(const vec<N, T> &l, const vec<N, T> &r) {
        return TransposeMul(l, r);
    }

    template <typename T>
    inline vec3<T> Cross(const vec3<T> &l, const vec3<T> &r) {
        return vec3<T>(l[1] * r[2] - l[2] * r[1],
                       l[2] * r[0] - l[0] * r[2],
                       l[0] * r[1] - l[1] * r[0]);
    }

    template <typename T>
    inline vec3<T> Reflect(const vec3<T> i, const vec3<T> n) {
        single in = Dot(i, n);
        return n * (AsSingle(-2) * in) + i;
    }
}  // namespace igi
