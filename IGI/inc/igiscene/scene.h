#pragma once

#include "igiscene/IAggregate.h"

namespace igi {
    class scene {
        IAggregate& _aggregate;

        color_rgb _background;

    public:
        scene(IAggregate& aggregate, color_rgb background = palette_rgb::black)
            : _aggregate(aggregate), _background(background) {}

        const IAggregate& getAggregate() const { return _aggregate; }

        color_rgb getBackground() const { return _background; }
    };
}
