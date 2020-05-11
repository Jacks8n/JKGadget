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

        virtual bound_t getBound() const = 0;

        virtual bool isHit(const ray &r) const                      = 0;
        virtual bool tryHit(ray &r, surface_interaction *res) const = 0;
    };

    class surface_helper {
      public:
        template <typename T,
                  std::enable_if_t<std::is_base_of_v<ISurface, T>, size_t> = 0>
        static constexpr surface_interaction CalculateInteraction(
            const T *const surf, const ray &r, const vec3f &normal, const vec2f &uv) {
            return surface_interaction(r.getEndpoint(),
                                       MakeReversedOrient(r.getDirection(), normal),
                                       surf->getPartialU(uv[0], uv[1]),
                                       surf->getPartialV(uv[0], uv[1]),
                                       uv);
        }
    };
}  // namespace igi
