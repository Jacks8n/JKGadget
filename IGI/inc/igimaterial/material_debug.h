#pragma once

#include "igimaterial/IMaterial.h"

namespace igi {
    class material_debug : public IMaterial {
      public:
        virtual color_rgb operator()(const vec3f &i, const vec3f &o, const vec3f &n) const override {
            return color_rgb(1_col, 0_col, 0_col);
        }

        virtual color_rgb getLuminance() const override {
            return palette_rgb::yellow;
        }

        virtual scatter getScatter(const vec3f &i, const mat3x3f &tanCoord, random_engine_t &rand) const override {
            hemisphere_cos_distribution hcd;
            scatter s;
            s.direction = tanCoord * hcd(rand, &s.pdf);
            return s;
        }
    };
}  // namespace igi
