#pragma once

#include <cmath>
#include "igigeometry/ISurface.h"
#include "igimath/const.h"

namespace igi {
    class sphere : public ISurface {
        single _r;

      public:
        sphere(single r) : _r(r) { }

        single getArea() const override { return PiFour * _r * _r; }

        bound_t getBound(const transform &trans) const override {
            return bound_t(vec3f::One(_r)).transform(trans);
        }

        bool isHit(const ray &r, const transform &trans) const override;
        bool tryHit(ray &r, const transform &trans, surface_interaction *res) const override;
    };

}  // namespace igi
