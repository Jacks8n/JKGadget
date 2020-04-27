#pragma once

#include <cmath>
#include "igigeometry/ISurface.h"
#include "igimath/const.h"

namespace igi {
    class sphere : public ISurface {
        single _r;

    public:
        sphere(single r = 1) : _r(r) {}

        ~sphere() = default;

        vec3f getPoint(single u, single v) const override {
            return getNormal(u, v) * _r;
        }

        vec3f getPartialU(single u, single v) const override {
            single r = cos(v);
            return vec3f(-sin(u) * r, cos(u) * r, 0);
        }
        vec3f getPartialV(single u, single v) const override {
            single r = -sin(v);
            return vec3f(cos(u) * r, sin(u) * r, cos(v));
        }

        vec3f getNormal(single u, single v) const override {
            single r = cos(v);
            return vec3f(cos(u) * r, sin(u) * r, sin(v));
        }

        single getArea() const override { return PiFour * _r * _r; }

        bool isHit(const ray& r) const override;
        bool tryHit(ray& r, surface_hit &res) const override;
    };

} // namespace igi
