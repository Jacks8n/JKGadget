#pragma once

#include <memory_resource>
#include "igicontext.h"
#include "igimath/transform.h"

namespace igi {
    class transformable_base {
        static inline std::pmr::vector<transform> Transfoms { context::GetTypedAllocator<transform>() };

        transform &_transform;

      public:
        transformable_base() : _transform(Transfoms.emplace_back()) { }
        transformable_base(transform &transform) : _transform(transform) { }

        igi::transform &getTransform() {
            return _transform;
        }

        const igi::transform &getTransform() const {
            return _transform;
        }
    };
}  // namespace igi
