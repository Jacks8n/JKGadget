#include <cmath>
#include "igigeometry/sphere.h"

namespace igi {
    bool sphere::isHit(const ray& r) const {
        single p = -Dot(r.getDirection(), r.getOrigin());
        return Lesscf(r.cast(p).magnitudeSqr(), _r * _r) &&
            Lesscf(p, r.getT() * r.getDirection().magnitudeSqr());
    }

    bool sphere::tryHit(ray& r, surface_hit& res) const {
        single a = r.getDirection().magnitudeSqr();
        single b = AsSingle(2) * Dot(r.getDirection(), r.getOrigin());
        single c = r.getOrigin().magnitudeSqr() - _r * _r;
        single d = b * b - 4 * a * c;
        if (!IsPositivecf(d))
            return false;

        single div = AsSingle(.5) / a;
        b = -b * div;
        d = sqrt(d) * div;

        if (InRangecf(0, r.getT(), b - d))
            r.setT(b - d);
        else if (InRangecf(0, r.getT(), b + d))
            r.setT(b + d);
        else
            return false;

        res.position = r.getEndpoint();
        res.normal = res.position * (AsSingle(1) / _r);
        return true;
    }
} // namespace igi
