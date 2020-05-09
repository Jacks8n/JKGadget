#pragma once

#include "igientity/entity.h"

namespace igi {
    struct ISBVHItem {
        virtual entity &getEntity() const = 0;

        virtual bool canSplit() const = 0;

        virtual void splitBound(single coord, size_t dim) const = 0;
    };
}  // namespace igi