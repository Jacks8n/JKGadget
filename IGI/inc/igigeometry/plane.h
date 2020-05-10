#include "igigeometry/ISurface.h"
#include "igimath/quaternion.h"

namespace igi {
    class plane : public ISurface {
        vec3f _origin;
        vec3f _normal;
        vec3f _tangentU, _tangentV;

      public:
        plane(const vec3f &tanU, const vec3f &tanV, const vec3f &origin = vec3f::One(0))
            : _origin(origin), _normal(Cross(tanU, tanV)),
              _tangentU(tanU), _tangentV(tanV) { }

        vec3f getPoint(single u, single v) const override {
            return _tangentU * ZeroOneToInf(u)
                   + _tangentV * ZeroOneToInf(v) + _origin;
        }

        vec3f getNormal(single u, single v) const override {
            return _normal;
        }

        vec3f getPartialU(single u, single v) const override;
        vec3f getPartialV(single u, single v) const override;

        single getArea() const override {
            return SingleInfinity;
        }

        bound_t getBound() const override {
            return bound_t::Inf();
        }

        bool isHit(const ray &r) const;
        bool tryHit(ray &r, surface_interaction *res) const;

      private:
        static constexpr single ZeroOneToInf(single val) {
            return (AsSingle(.5) - val) * AsSingle(.25) / (val * val - val);
        }

        static constexpr single ZeroOneToInf(single val) {
            return (AsSingle(.5) - val) * AsSingle(.25) / (val * val - val);
        }
    };
}  // namespace igi