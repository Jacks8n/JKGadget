#pragma once

#include "igimath/transform.h"
#include "igimath/vec.h"

namespace igi {
    class ray {
        vec3f _o, _d;

        single _t;

      public:
        ray()            = default;
        ray(const ray &) = default;
        ray(ray &&)      = default;
        constexpr ray(const vec3f &o, const vec3f &d, single t = SingleLarge)
            : _o(o), _d(d), _t(t) { }

        constexpr ray &operator=(const ray &) = default;
        constexpr ray &operator=(ray &&) = default;

        ~ray() = default;

        constexpr const vec3f &getOrigin() const { return _o; }

        void setOrigin(const vec3f &o) { _o = o; }

        constexpr const vec3f &getDirection() const { return _d; }

        void setDirection(const vec3f &d) { _d = d; }

        void setEndpoint(const vec3f &e) {
            setDirection(e - _o);
            setT(1);
        }

        constexpr single getTMin() const { return 0; }

        constexpr single getT() const { return _t; }

        void setT(single t) { _t = t; }

        void reset(const vec3f &o, const vec3f &d, single t = SingleLarge) {
            _o = o;
            _d = d;
            _t = t;
        }

        void normalizeDirection() {
            single m = _d.magnitude();
            _t *= m;
            _d = _d / m;
        }

        constexpr vec3f cast(single t) const { return _d * t + _o; }

        constexpr vec3f getEndpoint() const { return cast(_t); }
    };
}  // namespace igi
