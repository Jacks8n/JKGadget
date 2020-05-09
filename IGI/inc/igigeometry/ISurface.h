#pragma once

#include "igigeometry/bound.h"
#include "igigeometry/ray.h"
#include "igigeometry/surface_interaction.h"

namespace igi {
    struct ISurface {
        virtual vec3f getPoint(single u, single v) const = 0;

        virtual vec3f getNormal(single u, single v) const   = 0;
        virtual vec3f getPartialU(single u, single v) const = 0;
        virtual vec3f getPartialV(single u, single v) const = 0;

        virtual single getArea() const = 0;

        virtual bound_t getBound(const transform &trans) const = 0;

        virtual bool isHit(const ray &r) const                      = 0;
        virtual bool tryHit(ray &r, surface_interaction *res) const = 0;
    };
}  // namespace igi
