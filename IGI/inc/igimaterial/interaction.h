#pragma once

#include "igimath/vec.h"
#include "igimaterial/IMaterial.h"

namespace igi {
    struct interaction
    {
        const IMaterial* material;

        vec3f position;

        vec3f normal;
    };
}
