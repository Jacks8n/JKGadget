#include "igigeometry/sphere.h"
#include <cmath>

namespace igi {
    bool sphere::isHit(const ray &wr, const transform &trans) const {
        ray r    = surface_helper::ToLocalRay(r, trans);
        single p = -Dot(r.getOrigin(), r.getDirection());
        return Lesscf(r.cast(p).magnitudeSqr(), _r * _r)
               && Lesscf(p, r.getT() * r.getDirection().magnitudeSqr());
    }

    bool sphere::tryHit(ray &wr, const transform &trans, surface_interaction *res) const {
        ray r = surface_helper::ToLocalRay(wr, trans);

        single a = r.getDirection().magnitudeSqr();
        single b = 2_sg * Dot(r.getDirection(), r.getOrigin());
        single c = r.getOrigin().magnitudeSqr() - _r * _r;
        single d = b * b - 4_sg * a * c;

        if (!IsPoscf(d)) return false;

        a = .5_sg / a;
        b = -b * a;
        d = sqrt(d) * a;

        if (InRangecf(r.getTMin(), r.getT(), b - d))
            wr.setT(b - d);
        else if (InRangecf(r.getTMin(), r.getT(), b + d))
            wr.setT(b + d);
        else
            return false;

        res->normal   = r.getEndpoint().normalized();
        res->position = res->normal * _r;

        vec2f xy(res->normal);
        single sinu       = xy.magnitude();
        single cosu       = res->normal[2];
        auto [cosv, sinv] = (xy * (1_sg / sinu)).rows();

        res->uv = vec2f(Saturate(atan2(sinu, cosu) * PiInv),
                        Saturate(PiTwoToZeroOne(atan2(res->normal[1], res->normal[0]))));

        res->dpdu = vec3f(cosv * cosu * _r, sinv * cosu * _r, -sinu * _r);
        res->dpdv = vec3f(-sinv * sinu * _r, cosv * sinu * _r, 0_sg);

        res->normal = MakeReversedOrient(r.getDirection(), res->normal);

        surface_helper::ResToWorldSpace(trans, res);
        return true;
    }
}  // namespace igi
