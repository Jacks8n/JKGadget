#pragma once

#include "igimath/vec.h"

namespace igi {
    struct surface_interaction {
        vec3f position;

        vec3f normal;

        vec3f dpdu, dpdv;

        vec2f uv;

        surface_interaction() = default;
        constexpr surface_interaction(const vec3f &pos, const vec3f &norm,
                                      const vec3f &dpdu, const vec3f &dpdv, const vec2f uv)
            : position(pos), normal(norm), dpdu(dpdu), dpdv(dpdv), uv(uv) { }

        // normalization might be needed
        constexpr mat3x3f getNormalSpace() const {
            return mat3x3f(dpdu, dpdv, normal);
        }
    };
}  // namespace igi
