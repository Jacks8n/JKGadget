#pragma once

#include "igimath/vec.h"

namespace igi {
    class quaternion {
        friend quaternion operator+(const quaternion &l, const quaternion &r);
        friend quaternion operator-(const quaternion &l, const quaternion &r);
        friend quaternion operator*(const quaternion &q, single s);

        single _x, _y, _z, _w;

      public:
        constexpr quaternion(single x, single y, single z, single w)
            : _x(x), _y(y), _z(z), _w(w) { }
        constexpr quaternion(single r, const vec3f &i)
            : _x(r), _y(i[0]), _z(i[1]), _w(i[2]) { }
        constexpr quaternion(const vec3f &v)
            : quaternion(0, v) { }

        static constexpr quaternion Identity() {
            return quaternion(0, 0, 0, 1);
        }

        static quaternion AxisRotation(const vec3f axis, single ang) {
            ang *= .5_sg;
            return quaternion(std::cos(ang), axis * std::sin(ang));
        }

        constexpr vec3f getAxis() const {
            return getImaginary();
        }

        single magnitude() const {
            return asVec4f().magnitude();
        }

        constexpr single magnitudeSqr() const {
            return asVec4f().magnitudeSqr();
        }

        constexpr quaternion conjunction() const {
            return quaternion(_x, -getImaginary());
        }

        constexpr quaternion inverse() const {
            return conjunction() * (1_sg / magnitudeSqr());
        }

        constexpr quaternion operator*(const quaternion &r) const {
            return quaternion(_x * r._x - Dot(getImaginary(), r.getImaginary()),
                              r.getImaginary() * _x + getImaginary() * r._x
                                  + Cross(getImaginary(), r.getImaginary()));
        }

        quaternion &operator*=(const quaternion &r) {
            *this = *this * r;
        }

        constexpr vec3f operator*(const vec3f &v) const {
            return (*this * quaternion(v) * *this).getImaginary();
        }

      private:
        constexpr vec4f asVec4f() const {
            return vec4f(_x, _y, _z, _w);
        }

        constexpr vec3f getImaginary() const {
            return vec3f(_y, _z, _w);
        }
    };

    constexpr quaternion operator+(const quaternion &l, const quaternion &r) {
        return quaternion(l._x + r._x, l._y + r._y, l._z + r._z, l._w + r._w);
    }

    constexpr quaternion operator-(const quaternion &l, const quaternion &r) {
        return quaternion(l._x - r._x, l._y - r._y, l._z - r._z, l._w - r._w);
    }

    constexpr quaternion operator*(const quaternion &q, single s) {
        return quaternion(q._x * s, q._y * s, q._z * s, q._w * s);
    }
}  // namespace igi