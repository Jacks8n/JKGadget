#pragma once

#include "igiscene/scene.h"

namespace igi {
    struct IIntegrator {
        virtual color_rgb integrate(ray &r, pcg32 &pcg) const = 0;
    };
}  // namespace igi
