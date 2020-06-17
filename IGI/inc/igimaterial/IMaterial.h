#pragma once

#include "igigeometry/ray.h"
#include "igimath/random.h"
#include "igitexture/color.h"

namespace igi {
    struct scatter {
        vec3f direction;

        single pdf;

        scatter() : direction(0_sg, 0_sg, 0_sg), pdf(0_sg) { }
        scatter(const igi::vec3f &d, single p) : direction(d), pdf(p) { }

        scatter &operator=(const scatter &) = default;
    };

    struct IMaterial {
        META_BE_RT(IMaterial)

        virtual color3 operator()(const vec3f &i, const vec3f &o, const vec3f &n) const = 0;

        virtual color3 getLuminance() const = 0;

        virtual scatter getScatter(const vec3f &i, const mat3x3f &tanCoord, pcg32 &rand) const = 0;
    };
}  // namespace igi
