#include "igigeometry/sphere.h"
#include <cmath>

namespace igi {
    bool sphere::isHit(const ray &r) const {
        single p = -Dot(r.getOrigin(), r.getDirection());
        return Lesscf(r.cast(p).magnitudeSqr(), _r * _r)
               && Lesscf(p, r.getT() * r.getDirection().magnitudeSqr());
    }

    bool sphere::tryHit(ray &r, surface_interaction *res) const {
        single a = r.getDirection().magnitudeSqr();
        single b = 2_sg * Dot(r.getDirection(), r.getOrigin());
        single c = r.getOrigin().magnitudeSqr() - _r * _r;
        single d = b * b - 4_sg * a * c;

        if (!IsPositivecf(d)) return false;

        a = .5_sg / a;
        b = -b * a;
        d = sqrt(d) * a;

        if (InRangecf(r.getTMin(), r.getT(), b - d))
            r.setT(b - d);
        else if (InRangecf(r.getTMin(), r.getT(), b + d))
            r.setT(b + d);
        else
            return false;

        res->position = r.getEndpoint();

        res->normal = res->position * 1_sg / _r;
        if (IsPositivecf(Dot(res->normal, r.getDirection())))
            res->normal = -res->normal;

        // Radius of latitude
        single rla = static_cast<vec2f>(res->normal).magnitude();

        res->uv = vec2f(Saturate(atan2(rla, res->normal[2]) * PiInv),
                        Saturate(PiTwoToZeroOne(atan2(res->normal[1], res->normal[0]))));

        res->dpdu = getPartialU(res->uv[0], res->uv[1]);
        res->dpdv = getPartialV(res->uv[0], res->uv[1]);

        return true;
    }
}  // namespace igi
