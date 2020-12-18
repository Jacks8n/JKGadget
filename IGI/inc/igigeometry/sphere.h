#pragma once

#include <cmath>
#include "igigeometry/ISurface.h"
#include "igimath/const.h"

namespace igi {
    class sphere : public ISurface {
        single _r;

      public:
        META_BE_RT(sphere, ser_pmr_name_a("sphere"), deser_pmr_func_a<ISurface>([](const serializer_t &ser) {
                       IGI_SERIALIZE_OPTIONAL(single, radius, 1_sg, ser);

                       sphere *p = context::Allocate<sphere>();
                       context::Construct(p, radius);
                       return static_cast<ISurface *>(p);
                   }))

        sphere(single r) : _r(r) { }

        single getArea() const override { return PiFour * _r * _r; }

        bound_t getBound(const transform &trans) const override {
            return bound_t(vec3f::One(_r)).transform(trans);
        }

        bool isHit(const ray &r, const transform &trans) const override;
        bool tryHit(ray &r, const transform &trans, surface_interaction *res) const override;
    };

}  // namespace igi
