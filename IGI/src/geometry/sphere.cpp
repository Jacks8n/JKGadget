#include "igigeometry/sphere.h"
#include <cmath>

namespace igi {
    bool sphere::isHit(const ray &wr, const transform &trans) const {
        const ray r = surface_helper::ToLocalRay(wr, trans);
        single p    = -Dot(r.getOrigin(), r.getDirection());
        return r.cast(p).magnitudeSqr() < _r * _r
               && p < r.getT() * r.getDirection().magnitudeSqr();
    }

    bool sphere::tryHit(ray &wr, const transform &o2w, surface_interaction *res) const {
        const ray r = surface_helper::ToLocalRay(wr, o2w);

        esingle a = r.getDirection().magnitudeSqr();
        esingle b = 2_sg * Dot(r.getDirection(), r.getOrigin());
        esingle c = r.getOrigin().magnitudeSqr() - _r * _r;
        esingle d = b * b - 4_sg * a * c;

        if (!(d > 0)) return false;

        a = .5_sg / a;
        b = -b * a;
        d = sqrt(d) * a;

        esingle t;
        if (wr.occlude(b - d))
            t = b - d;
        else if (wr.occlude(b + d))
            t = b + d;
        else
            return false;

        wr.setT(t);
        res->position = r.cast(t);
        res->normal   = res->position.normalized();

        vec2f xy(res->normal);
        single sinu       = xy.magnitude();
        single cosu       = res->normal[2];
        auto [cosv, sinv] = (xy * (1_sg / sinu)).asTuple();

        res->uv = vec2f(Saturate(atan2(sinu, cosu) * PiInv),
                        Saturate(PiTwoToZeroOne(atan2(res->normal[1], res->normal[0]))));

        res->dpdu = vec3f(cosv * cosu * _r, sinv * cosu * _r, -sinu * _r);
        res->dpdv = vec3f(-sinv * sinu * _r, cosv * sinu * _r, 0_sg);

        res->normal = MakeReversedOrient(r.getDirection(), res->normal);

        surface_helper::ResToWorldSpace(o2w, res);
        return true;
    }
}  // namespace igi
