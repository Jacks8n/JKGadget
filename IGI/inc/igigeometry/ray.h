#pragma once

#include "igimath/transform.h"
#include "igimath/vec.h"

namespace igi {
    class ray {
        static constexpr single DefaultT = SingleLarge;

        vec3f _o, _d;

        esingle _t;

        /// ===================================================
        /// Factors relating to the transformation to ray space

        int _perm0, _perm1, _perm2;

        vec3f _shearCol;

        /// ===================================================

      public:
        ray()            = default;
        ray(const ray &) = default;
        ray(ray &&)      = default;
        constexpr ray(const vec3f &o, const vec3f &d, single t = DefaultT)
            : _o(o), _d(d), _t(t), _perm0(MaxIcf(d[0], d[1], d[2])),
              _perm1((_perm0 + 1) % 3), _perm2((_perm0 + 2) % 3),
              _shearCol(-d[0] / d[2], -d[1] / d[2], 1_sg / d[2]) { }

        constexpr ray &operator=(const ray &) = default;
        constexpr ray &operator=(ray &&) = default;

        ~ray() = default;

        constexpr const vec3f &getOrigin() const { return _o; }

        ray &setOrigin(const vec3f &o) {
            _o = o;
            return *this;
        }

        constexpr const vec3f &getDirection() const { return _d; }

        ray &setDirection(const vec3f &d) {
            _d     = d;
            _perm0 = MaxIcf(d[0], d[1], d[2]);
            _perm1 = (_perm0 + 1) % 3;
            _perm2 = (_perm0 + 2) % 3;
            return *this;
        }

        ray &setEndpoint(const vec3f &e) {
            setDirection(e - _o);
            setT(1);
            return *this;
        }

        constexpr single getTMin() const { return 0_sg; }

        constexpr const esingle &getT() const { return _t; }

        constexpr bool isNearerT(esingle t) const {
            return InRangecf(getTMin(), getT(), t);
        }

        ray &setT(esingle t) {
            _t = t;
            return *this;
        }

        ray &reset(const vec3f &o, const vec3f &d, esingle t = DefaultT) {
            return setOrigin(o).setDirection(d).setT(t);
        }

        void normalizeDirection() {
            single m = _d.magnitude();
            _t *= m;
            _d = _d / m;
        }

        constexpr vec3f cast(single t) const { return _d * t + _o; }

        constexpr vec3f getEndpoint() const { return cast(_t); }

        vec3f toRaySpace(const vec3f &p) const {
            vec3f t = (p - _o).permute(_perm0, _perm1, _perm2);
            return _shearCol * t[2] + t;
        }
    };
}  // namespace igi
