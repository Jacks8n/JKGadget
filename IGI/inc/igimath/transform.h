#pragma once

#include "igimath/mat4x4f.h"
#include "igimath/vec.h"

namespace igi {
    class transform {
        mat4x4f _mat, _inv;

        transform(const mat4x4f &mat, const mat4x4f &inv) : _mat(mat), _inv(inv) { }

      public:
        META_BE(transform, rflite::func_a([](const serializer_t &ser) {
                    IGI_SERIALIZE_OPTIONAL(vec3f, position, vec3f::One(0_sg), ser);
                    IGI_SERIALIZE_OPTIONAL(vec3f, rotation, vec3f::One(0_sg), ser);
                    IGI_SERIALIZE_OPTIONAL(vec3f, scale, vec3f::One(1_sg), ser);
                    return has_scale ? rflite::meta_helper::any_ins<transform>(position, rotation, scale)
                                     : has_rotation ? rflite::meta_helper::any_ins<transform>(position, rotation)
                                                    : has_position ? rflite::meta_helper::any_ins<transform>(position)
                                                                   : rflite::meta_helper::any_ins<transform>();
                }));

        transform() : transform(mat4x4f::Identity(), mat4x4f::Identity()) { }
        transform(const vec3f &pos) : transform() { translation(pos); }
        transform(const vec3f &pos, const vec3f &rot) : transform() { rotation(rot).translation(pos); }
        transform(const vec3f &pos, const vec3f &rot, const vec3f &s) : transform() { scale(s).rotation(rot).translation(pos); }

        const mat4x4f &getMat() const {
            return _mat;
        }

        const mat4x4f &getInv() const {
            return _inv;
        }

        transform &inverse() {
            swap(_mat, _inv);
            return *this;
        }

        vec3f mulPos(const vec3f &v) const {
            return _mat.mulPos(v);
        }

        vec3f mulVec(const vec3f &v) const {
            return _mat.mulVec(v);
        }

        vec3f mulPosInv(const vec3f &v) const {
            return _inv.mulPos(v);
        }

        vec3f mulVecInv(const vec3f &v) const {
            return _inv.mulVec(v);
        }

        vec3f mulNormal(const vec3f &n) const {
            mat4x4f m = _inv.transpose();
            return m.mulVec(n);
        }

        transform &translation(const vec3f &t) {
            mat4x4f m(1, 0, 0, t[0],
                      0, 1, 0, t[1],
                      0, 0, 1, t[2],
                      0, 0, 0, 1);

            _mat        = m * _mat;
            m.get(0, 3) = -m.get(0, 3);
            m.get(1, 3) = -m.get(1, 3);
            m.get(2, 3) = -m.get(2, 3);
            _inv        = _inv * m;
            return *this;
        }

        transform &rotationX(single x) {
            single c = cos(x), s = sin(x);
            mat4x4f m(1, 0, 0, 0,
                      0, c, -s, 0,
                      0, s, c, 0,
                      0, 0, 0, 1);

            _mat = m * _mat;
            std::swap(m.get(1, 2), m.get(2, 1));
            _inv = _inv * m;
            return *this;
        }

        transform &rotationY(single y) {
            single c = cos(y), s = sin(y);
            mat4x4f m(c, 0, s, 0,
                      0, 1, 0, 0,
                      -s, 0, c, 0,
                      0, 0, 0, 1);

            _mat = m * _mat;
            std::swap(m.get(0, 2), m.get(2, 0));
            _inv = _inv * m;
            return *this;
        }

        transform &rotationZ(single z) {
            single c = cos(z), s = sin(z);
            mat4x4f m(c, -s, 0, 0,
                      s, c, 0, 0,
                      0, 0, 1, 0,
                      0, 0, 0, 1);

            _mat = m * _mat;
            std::swap(m.get(0, 1), m.get(1, 0));
            _inv = _inv * m;
            return *this;
        }

        transform &rotation(const vec3f &r) {
            return rotationX(r[0]).rotationY(r[1]).rotationZ(r[2]);
        }

        transform &scale(const vec3f &s) {
            for (size_t i = 0; i < 3; i++)
                _mat.get(i, i) *= s[i];
            return *this;
        }

        transform &scale(single s) {
            for (size_t i = 0; i < 3; i++)
                _mat.get(i, i) *= s;
            return *this;
        }

        vec4f operator*(const vec4f &v) const {
            return _mat * v;
        }

        mat4x4f operator*(const mat4x4f &m) const {
            return _mat * m;
        }

        transform &operator*=(const transform &t) {
            _mat = _mat * t._mat;
            _inv = t._inv * _inv;
            return *this;
        }
    };
}  // namespace igi
