#pragma once

#include "igiscene/aggregate.h"

namespace igi {
    class scene {
        const aggregate &_aggregate;

        color3 _background;

      public:
        scene(const aggregate &aggregate, color3 background = palette::black)
            : _aggregate(aggregate), _background(background) { }

        const aggregate &getAggregate() const { return _aggregate; }

        color3 getBackground() const { return _background; }
    };
}  // namespace igi
