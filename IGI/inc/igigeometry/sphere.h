#pragma once

#include <cmath>
#include "igigeometry/ISurface.h"
#include "igimath/const.h"

namespace igi {
    class sphere : public ISurface {
        single _r;

      public:
        sphere(single r) : _r(r) { }

        vec3f getPoint(single u, single v) const override {
            return getNormal(u, v) * _r;
        }

        vec3f getPartialU(single u, single v) const override {
            u *= Pi, v = ZeroOneToPiTwo(v);
            single c = cos(u);
            return vec3f(cos(v) * c, sin(v) * c, -sin(u));
        }

        vec3f getPartialV(single u, single v) const override {
            u *= Pi, v = ZeroOneToPiTwo(v);
            single s = sin(u);
            return vec3f(-sin(v) * s, cos(v) * s, 0);
        }

        vec3f getNormal(single u, single v) const override {
            u *= Pi, v = ZeroOneToPiTwo(v);
            single s = sin(u);
            return vec3f(cos(v) * s, sin(v) * s, cos(u));
        }

        single getArea() const override { return PiFour * _r * _r; }

        bool isHit(const ray& r) const override;
        bool tryHit(ray& r, surface_interaction* res) const override;
    };

}  // namespace igi
