#pragma once

#include "igimath/ray.h"
#include "igitexture/color.h"

namespace igi {
    struct scatter {
        vec3f direction;

        single probability;

        scatter() = default;
        scatter(const igi::vec3f& d, single p) : direction(d), probability(p) {}

        scatter& operator=(const scatter&) = default;
    };

    struct IMaterial {
        virtual color_rgb operator()(const vec3f& i, const vec3f& o, const vec3f& n) const = 0;

        virtual color_rgb getLuminance() const = 0;

        virtual scatter getScatter(const vec3f& i, const vec3f& n) const = 0;
    };
}
