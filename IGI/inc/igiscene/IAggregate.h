#pragma once

#include "igimath/ray.h"
#include "igimaterial/interaction.h"
#include "igientity/entity.h"

namespace igi {
    struct IAggregate
    {
        virtual void add(entity& e) = 0;
        virtual void addRange(size_t n, entity* es) = 0;

        virtual bool isHit(const ray& r) const = 0;
        virtual bool tryHit(ray& r, interaction& res) const = 0;
    };
}