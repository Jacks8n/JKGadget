#pragma once

#include "igientity/entity.h"

namespace igi {
    struct IAggregateItem {
        virtual const entity &getEntity() const = 0;

        virtual void splitBound() const = 0;
    };
}  // namespace igi