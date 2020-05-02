#pragma once

#include "igimaterial/IMaterial.h"

namespace igi {
    class material_emissive : public IMaterial {
        color_rgb _e;

      public:
        material_emissive(single il = 1, const color_rgb &e = palette_rgb::white)
            : _e(ToBaseColor(e) * il) { }

        color_rgb operator()(const vec3f &i, const vec3f &o, const vec3f &n) const override {
            return palette_rgb::black;
        }

        color_rgb getLuminance() const override {
            return _e;
        }

        scatter getScatter(const vec3f &i, const mat3x3f& tanCoord, pcg32 &rand) const override {
            return scatter(vec3f::One(0), 0);
        }
    };
}  // namespace igi
