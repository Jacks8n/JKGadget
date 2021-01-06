#pragma once

#include "igiutilities/igiassert.h"
#include "igimath/transform.h"
#include "igimath/vec.h"

namespace igi {
    class ray {
        static constexpr single DefaultT = SingleLarge;

        vec3f _o, _d;

        esingle _t;

        int _permZ, _permY, _permX;

        single _invDZ;

        // -d.x / d.z, d.y / d.z
        vec2f _shear;

      public:
        ray()            = default;
        ray(const ray &) = default;
        ray(ray &&)      = default;

        constexpr ray(const vec3f &o, const vec3f &d, single t = DefaultT)
            : _o(o), _d(d), _t(t),
              _permZ(MaxIcf(d[0], d[1], d[2])),
              _permY(_permZ ? 3 - _permZ : 2),
              _permX(_permZ == 2 ? 0 : _permZ + 1),
              _invDZ(1_sg / _d[_permZ]),
              _shear(-d[_permX] * _invDZ, -d[_permY] * _invDZ) { }

        constexpr ray &operator=(const ray &) = default;
        constexpr ray &operator=(ray &&) = default;

        ~ray() = default;

        constexpr const vec3f &getOrigin() const {
            return _o;
        }

        void setOrigin(const vec3f &o) {
            new (this) ray(o, getEndpoint() - o, 1_sg);
        }

        constexpr const vec3f &getDirection() const {
            return _d;
        }

        void setDirection(const vec3f &d) {
            new (this) ray(_o, d, _t);
        }

        constexpr vec3f getEndpoint() const {
            return cast(_t);
        }

        void setEndpoint(const vec3f &e) {
            new (this) ray(_o, e - _o, 1_sg);
        }

        constexpr single getTMin() const {
            return 0_sg;
        }

        constexpr const esingle &getT() const {
            return _t;
        }

        void setT(esingle t) {
            igiassert(t > getTMin());

            _t = t;
        }

        constexpr bool occluded(esingle t) const {
            return InRangecf(getTMin(), getT(), t);
        }

        void reset(const vec3f &o, const vec3f &e) {
            new (this) ray(o, e - o, 1_sg);
        }

        void normalizeDirection() {
            single m = _d.magnitude();
            _t *= m;
            _d = _d / m;
        }

        constexpr vec3f cast(single t) const {
            return _d * t + _o;
        }

        /// @brief shear ray direction to (0, 0, 1), non-affine
        vec3f toRaySpace(const vec3f &p) const {
            vec3f t = (p - _o).permute(_permX, _permY, _permZ);
            t.row<0, 1>() += _shear * t[2];
            t[2] *= _invDZ;
            return t;
        }

      private:
        void assertNormalizdDir() const {
            igiassert(_d.isNormalized());
        }
    };
}  // namespace igi
