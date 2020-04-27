#pragma once

#include "igimath/vec.h"
#include "igimath/transform.h"

namespace igi {
    class ray {
        vec3f _o, _d;

        single _t = SingleMax;

    public:
        ray() = default;
        ray(const ray&) = default;
        ray(ray&&) = default;
        ray(const vec3f& o, const vec3f& d, single t = SingleMax)
            : _o(o), _d(d), _t(t) {}

        ray& operator=(const ray&) = default;
        ray& operator=(ray&&) = default;

        ~ray() = default;

        const vec3f& getOrigin() const { return _o; }

        void setOrigin(const vec3f& o) { _o = o; }

        const vec3f& getDirection() const { return _d; }

        void setDirection(const vec3f& d) { _d = d; }

        void setEndpoint(const vec3f& e) {
            setDirection(e - _o);
            setT(1);
        }

        single getT() const { return _t; }

        void setT(single t) { _t = t; }

        void reset(const vec3f& o, const vec3f& d, single t = SingleMax) {
            _o = o;
            _d = d;
            _t = t;
        }

        void normalizeDirection() {
            single m = _d.magnitude();
            _t *= m;
            _d = _d / m;
        }

        vec3f cast(single t) const { return _d * t + _o; }

        vec3f getEndpoint() const { return cast(_t); }
    };
} // namespace igi
