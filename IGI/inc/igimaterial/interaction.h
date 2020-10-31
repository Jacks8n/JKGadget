#pragma once

#include "igigeometry/surface_interaction.h"
#include "igimaterial/IMaterial.h"
#include "igimath/vec.h"

namespace igi {
    class entity;

    struct interaction {
        using entity_id_t = uintptr_t;

        static constexpr entity_id_t EntityIDNull = ~0;

        entity_id_t entityId;

        const IMaterial *material;

        surface_interaction surface;

        static entity_id_t EntityToID(const entity *e) {
            return reinterpret_cast<entity_id_t>(e);
        }
    };
}  // namespace igi
