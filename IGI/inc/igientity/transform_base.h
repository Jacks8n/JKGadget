#pragma once

#include "igimath/transform.h"

namespace igi {
    class transformable_base {
        transform _transform;

      public:
        transformable_base() = default;
        explicit transformable_base(const transform &trans) : _transform(trans) { }

        igi::transform &getTransform() {
            return _transform;
        }

        const igi::transform &getTransform() const {
            return _transform;
        }
    };
}  // namespace igi
