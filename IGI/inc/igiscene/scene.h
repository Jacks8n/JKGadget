#pragma once

#include "igiscene/aggregate.h"

namespace igi {
    class scene {
        const aggregate &_aggregate;

        color_rgb _background;

      public:
        scene(const aggregate &aggregate, color_rgb background = palette_rgb::black)
            : _aggregate(aggregate), _background(background) { }

        const aggregate &getAggregate() const { return _aggregate; }

        color_rgb getBackground() const { return _background; }
    };
}  // namespace igi
