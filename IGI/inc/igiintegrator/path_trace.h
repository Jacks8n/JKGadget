#pragma once

#include <ctime>
#include "igiintegrator/IIntegrator.h"
#include "igimath/random.h"

namespace igi {
    class path_trace : public IIntegrator {
        const scene &_scene;

        const size_t _depth;

        const size_t _split;

        pcg32 _random;

      public:
        path_trace(const scene &scene, size_t depth = 4, size_t split = 8)
            : _scene(scene), _depth(depth), _split(split < 1 ? 1 : split), _random() { }

        color_rgb integrate(ray &r) override;

      private:
        color_rgb integrate_impl(const igi::vec3f &o, const igi::interaction &interaction, size_t depth);
    };
}  // namespace igi
