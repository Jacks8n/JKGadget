#pragma once

#include "igiintegrator/IIntegrator.h"
#include "igimath/random.h"

namespace igi {
    class path_trace : public IIntegrator {
        const scene &_scene;

        const size_t _depth;

        const size_t _split;

      public:
        path_trace(const scene &scene, size_t depth = 4, size_t split = 1)
            : _scene(scene), _depth(depth), _split(split < 1 ? 1 : split) { }

        color3 integrate(ray &r, integrator_context &context) const override {
            interaction i;
            return _scene.getAggregate().tryHit(r, &i, context.itrtmp)
                       ? integrate_impl(r.getDirection(), i, _depth, context)
                       : _scene.getBackground();
        }

      private:
        color3 integrate_impl(const igi::vec3f &o, const igi::interaction &interaction,
                              size_t depth, integrator_context &context) const;
    };
}  // namespace igi
