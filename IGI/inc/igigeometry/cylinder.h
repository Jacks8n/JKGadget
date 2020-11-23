#include "ISurface.h"
#include "igimath/const.h"

namespace igi {
    class cylinder : public ISurface {
        single _r;
        single _zMin;
        single _zMax;

      public:
        META_BE_RT(cylinder, ser_pmr_name_a("cylinder"), deser_pmr_func_a<ISurface>([](const serializer_t &ser) {
                       IGI_SERIALIZE_OPTIONAL(single, radius, 1_sg, ser);
                       IGI_SERIALIZE_OPTIONAL(single, min, .5_sg, ser);
                       IGI_SERIALIZE_OPTIONAL(single, max, .5_sg, ser);

                       cylinder *p = context::Allocate<cylinder>();
                       context::Construct(p, radius, min, max);
                       return p;
                   }))

        cylinder(single r, single zMin, single zMax)
            : _r(r), _zMin(zMin), _zMax(zMax) {
            if (_zMax < _zMin)
                std::swap(_zMax, _zMin);
        }

        single getArea() const override {
            return abs(_zMax - _zMin) * PiTwo * _r;
        }

        virtual bound_t getBound(const transform &trans) const override {
            return bound_t(vec3f(-_r, -_r, _zMin), vec3f(_r, _r, _zMax)).transform(trans);
        }

        bool isHit(const ray &r, const transform &trans) const override;
        bool tryHit(ray &r, const transform &trans, surface_interaction *res) const override;
    };
}  // namespace igi
