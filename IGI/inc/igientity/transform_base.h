#pragma once

#include <memory_resource>
#include "igicontext.h"
#include "igimath/transform.h"
#include "igiutilities/igiassert.h"

namespace igi {
    class transformable_base {
        static inline std::pmr::vector<transform> Transforms { context::GetTypedAllocator<transform>() };

        transform &_transform;

      public:
        transformable_base() : _transform(Transforms.emplace_back()) { }
        transformable_base(transform &trans) : _transform(gatherTransform(trans)) { }

        igi::transform &getTransform() {
            return _transform;
        }

        const igi::transform &getTransform() const {
            return _transform;
        }

      private:
        static transform &gatherTransform(transform &trans) {
            if (Transforms.data() < &trans && &trans <= &Transforms.back())
                return trans;

            Transforms.emplace_back(trans);
            return Transforms.back();
        }
    };
}  // namespace igi
