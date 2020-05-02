#pragma once

#include "igimath/vec.h"

namespace igi {
    struct surface_interaction {
        vec3f position;

        vec3f normal;

        vec3f dpdu, dpdv;

        vec2f uv;
    };
}  // namespace igi
