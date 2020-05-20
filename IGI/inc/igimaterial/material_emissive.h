#pragma once

#include "igimaterial/IMaterial.h"

namespace igi {
    class material_emissive : public IMaterial {
        color3 _e;

      public:
        material_emissive(single il = 1, const color3 &e = palette::white)
            : _e(e.base() * il) { }

        color3 operator()(const vec3f &i, const vec3f &o, const vec3f &n) const override {
            return palette::black;
        }

        color3 getLuminance() const override {
            return _e;
        }

        scatter getScatter(const vec3f &i, const mat3x3f &tanCoord, random_engine_t &rand) const override {
            return scatter();
        }
    };
}  // namespace igi
