#pragma once

#include "igimath/ray.h"

namespace igi {
    struct surface_hit {
        vec3f position;

        vec3f normal;
    };

    struct ISurface {
        virtual vec3f getPoint(single u, single v) const = 0;

        virtual vec3f getPartialU(single u, single v) const = 0;
        virtual vec3f getPartialV(single u, single v) const = 0;

        virtual vec3f getNormal(single u, single v) const = 0;

        virtual single getArea() const = 0;

        virtual bool isHit(const ray& r) const = 0;
        virtual bool tryHit(ray& r, surface_hit &res) const = 0;
    };
} // namespace igi
