#pragma once

#include "igigeometry/bound.h"
#include "igigeometry/ray.h"
#include "igigeometry/surface_interaction.h"
#include "serialize.h"

namespace igi {
    struct ISurface {
        META_BE_RT(ISurface)

        virtual single getArea() const = 0;

        virtual bound_t getBound(const transform &trans) const = 0;

        virtual bool isHit(const ray &r, const transform &o2w) const                      = 0;
        virtual bool tryHit(ray &r, const transform &o2w, surface_interaction *res) const = 0;

      protected:
        class surface_helper {
          public:
            static ray ToLocalRay(const ray &wr, const transform &o2w) {
                ray r;
                r.setOrigin(o2w.mulPosInv(wr.getOrigin()))
                    .setEndpoint(o2w.mulPosInv(wr.getEndpoint()))
                    .normalizeDirection();
                return r;
            }

            static void ResToWorldSpace(const transform &o2w, surface_interaction *res) {
                res->position = o2w.mulPos(res->position);
                res->normal   = o2w.mulNormal(res->normal).normalized();
                res->dpdu     = o2w.mulVec(res->dpdu).normalized();
                res->dpdv     = o2w.mulVec(res->dpdv).normalized();
            }

            template <typename T, std::enable_if_t<std::is_base_of_v<ISurface, T>, size_t> = 0>
            static constexpr surface_interaction CalculateInteraction(
                const T *const surf, const ray &r, const vec3f &normal, const vec2f &uv) {
                return CalculateInteraction(surf, r, r.getEndpoint(), normal, uv);
            }

            template <typename T, std::enable_if_t<std::is_base_of_v<ISurface, T>, size_t> = 0>
            static constexpr surface_interaction CalculateInteraction(
                const T *surf, const ray &r, const vec3f &pos, const vec3f &normal, const vec2f &uv) {
                return surface_interaction(pos,
                                           MakeReversedOrient(r.getDirection(), normal),
                                           surf->getPartialU(uv[0], uv[1]),
                                           surf->getPartialV(uv[0], uv[1]),
                                           uv);
            }
        };
    };
}  // namespace igi
