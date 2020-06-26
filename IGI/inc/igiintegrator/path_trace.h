#pragma once

#include "igiintegrator/IIntegrator.h"
#include "igimath/random.h"

namespace igi {
    class path_trace : public IIntegrator {
        const size_t _depth;

        const size_t _split;

      public:
        META_BE(path_trace, rflite::func_a([](const serializer_t &ser) {
                    IGI_SERIALIZE_OPTIONAL(size_t, depth, 4, ser);
                    IGI_SERIALIZE_OPTIONAL(size_t, split, 1, ser);
                    return rflite::meta_helper::any_ins<path_trace>(depth, split);
                }))

        path_trace(size_t depth = 4, size_t split = 1)
            : _depth(depth), _split(split < 1 ? 1 : split) { }

        color3 integrate(const scene &scene, ray &r, integrator_context &context) const override {
            interaction i;
            return scene.getAggregate().tryHit(r, &i, context.itrtmp)
                       ? integrate_impl(scene, r.getDirection(), i, _depth, context)
                       : scene.getBackground();
        }

      private:
        color3 integrate_impl(const scene &scene, const vec3f &o, const interaction &interaction, size_t depth, integrator_context &context) const;
    };
}  // namespace igi
