#pragma once

#include "igiscene/aggregate.h"

namespace igi {
    class scene {
        std::shared_ptr<aggregate> _aggregate;

        color3 _background;

      public:
        scene(std::shared_ptr<aggregate> aggregate, color3 background = palette::black)
            : _aggregate(std::move(aggregate)), _background(background) { }

        const aggregate &getAggregate() const { return *_aggregate; }

        color3 getBackground() const { return _background; }
    };
}  // namespace igi
