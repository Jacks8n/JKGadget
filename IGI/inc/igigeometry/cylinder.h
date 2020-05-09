#include "ISurface.h"
#include "igimath/const.h"

namespace igi {
    class cylinder : public ISurface {
        single _r;
        single _zMin;
        single _zMax;

      public:
        cylinder(single r, single zMin, single zMax)
            : _r(r), _zMin(zMin), _zMax(zMax) {
            if (Lesscf(_zMax, _zMin))
                std::swap(_zMax, _zMin);
        }

        vec3f getPoint(single u, single v) const override {
            u = ZeroOneToPiTwo(u);
            return vec3f(_r * cos(u), _r * sin(u), Lerp(_zMin, _zMax, v));
        }

        vec3f getNormal(single u, single v) const override {
            u = ZeroOneToPiTwo(u);
            return vec3f(cos(u), sin(u), 0);
        }

        vec3f getPartialU(single u, single v) const override {
            u = ZeroOneToPiTwo(u);
            return vec3f(_r * sin(u), _r * sin(u), 0);
        }

        vec3f getPartialV(single u, single v) const override {
            return vec3f(0, 0, _zMax - _zMin);
        }

        single getArea() const override {
            return abs(_zMax - _zMin) * PiTwo * _r;
        }

        virtual bound_t getBound(const transform &trans) const override {
            return bound_t(vec3f(-_r, -_r, _zMin), vec3f(_r, _r, _zMax)).transform(trans);
        }

        bool isHit(const ray &r) const override;
        bool tryHit(ray &r, surface_interaction *res) const override;
    };
}  // namespace igi
