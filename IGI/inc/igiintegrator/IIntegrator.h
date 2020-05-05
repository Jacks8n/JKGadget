#pragma once

#include "igiscene/scene.h"
#include "igiaccleration/task_queue.h"

namespace igi {
    struct IIntegrator {
        virtual color_rgb integrate(ray& r) = 0;
    };
}