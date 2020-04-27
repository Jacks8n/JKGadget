#pragma once

#include <memory>
#include "igimath/transform.h"

namespace igi {
    struct ITransformable {
        virtual igi::transform& getTransform() = 0;

        virtual const igi::transform& getTransform() const = 0;
    };

    class transformable_base : public ITransformable {
        transform _transform;

    public:
        transformable_base() = default;

        igi::transform& getTransform() override {
            return _transform;
        }

        const igi::transform& getTransform() const override {
            return _transform;
        }
    };
}
