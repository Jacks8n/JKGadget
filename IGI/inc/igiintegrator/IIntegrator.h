#pragma once

#include "igiscene/scene.h"

namespace igi {
    struct IIntegrator {
        virtual color_rgb integrate(ray& r) = 0;
    };
}
