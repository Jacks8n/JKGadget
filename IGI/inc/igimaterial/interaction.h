#pragma once

#include "igimath/vec.h"
#include "igimaterial/IMaterial.h"
#include "igigeometry/surface_interaction.h"

namespace igi {
    struct interaction
    {
        const IMaterial* material;

        surface_interaction surface;
    };
}
