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

        vec3f getPartialU(single u, single v) const override {
            return _tangentU * ZeroOneToInfD(u);
        }

        vec3f getPartialV(single u, single v) const override {
            return _tangentV * ZeroOneToInfD(v);
        }

        single getArea() const override {
            return SingleInfinity;
        }

        bound_t getBound() const override {
            return bound_t::Inf();
        }

        bool isHit(const ray &r) const override {
            return IsPositivecf(Dot(r.getEndpoint() - _origin, _normal))
                   ^ IsPositivecf(Dot(r.getOrigin() - _origin, _normal));
        }

        bool tryHit(ray &r, surface_interaction *res) const override {
            single t = Dot(_origin - r.getOrigin(), _normal)
                       / Dot(_normal, r.getDirection());
            if (InRangecf(t, r.getTMin(), r.getT())) {
                r.setT(t);

                vec3f n = MakeReversedOrient(r.getDirection(), _normal);
                vec3f v = r.getEndpoint() - _origin;
                vec2f uv(Dot(_tangentU, v), Dot(_tangentV, v));

                *res = surface_helper::CalculateInteraction(this, r, n, uv);
                return true;
            }
            return false;
        }

      private:
        static constexpr single ZeroOneToInf(single val) {
            return (.5_sg - val) / (4_sg * (val * val - val));
        }

        static single ZeroOneToInfInv(single val) {
            single inv = .125_sg / val;
            return Saturate(.5_sg - inv + std::sqrt(.25_sg + inv * inv));
        }

        static constexpr single ZeroOneToInfD(single val) {
            return (2_sg * (val * val - val) + 1_sg)
                   / (8_sg * val * val * (val * val - 2_sg * val + 1_sg));
        }
    };
}  // namespace igi