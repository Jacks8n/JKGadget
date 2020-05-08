#pragma once

#include "igientity/entity.h"
#include "igigeometry/ray.h"
#include "igimaterial/interaction.h"

namespace igi {
    struct IAggregate {
        virtual void add(entity &e)                 = 0;
        virtual void addRange(size_t n, entity *es) = 0;

        virtual bool isHit(const ray &r) const              = 0;
        virtual bool tryHit(ray &r, interaction &res) const = 0;
    };
}  // namespace igi
