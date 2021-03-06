﻿#pragma once

#include "igimaterial/IMaterial.h"

namespace igi {
    class material_emissive : public IMaterial {
        color3 _e;

      public:
        META_BE_RT(material_emissive, ser_pmr_name_a("emissive"), deser_pmr_func_a<IMaterial>([](const serializer_t &ser) {
                       IGI_SERIALIZE_OPTIONAL(single, energy, 10_sg, ser);
                       IGI_SERIALIZE_OPTIONAL(color3, color, palette::white, ser);

                       IMaterial *p = context::New<material_emissive>(energy, color);
                       return p;
                   }))

        material_emissive(single il = 10_sg, const color3 &e = palette::white) : _e(e.base() * il) { }

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
