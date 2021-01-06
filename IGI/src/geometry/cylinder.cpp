#include "igigeometry/cylinder.h"

bool igi::cylinder::isHit(const ray &wr, const transform &trans) const {
    ray r = surface_helper::ToLocalRay(wr, trans);

    single zmin = r.getOrigin()[2], zmax = r.getEndpoint()[2];
    if (zmax < zmin)
        std::swap(zmax, zmin);
    if (!Overlapcf(zmin, zmax, _zMin, _zMax))
        return false;

    vec2f oxy = static_cast<vec2f>(r.getOrigin());
    vec2f dxy = static_cast<vec2f>(r.getDirection());
    single p  = -Dot(oxy, dxy);
    return (oxy + dxy * p).magnitudeSqr() < _r * _r;
}

bool igi::cylinder::tryHit(ray &wr, const transform &trans, surface_interaction *res) const {
    const ray r = surface_helper::ToLocalRay(wr, trans);

    vec2f oxy(r.getOrigin());
    vec2f dxy(r.getDirection());

    esingle a = Dot(dxy, dxy);
    esingle b = Dot(oxy, dxy) * 2_sg;
    esingle c = Dot(oxy, oxy) - _r * _r;

    auto [solved, t0, t1] = Quadratic(a, b, c);
    if (!solved)
        return false;

    const single &oz = r.getOrigin()[2];
    const single &dz = r.getDirection()[2];
    esingle t;
    if (wr.occluded(t0) && InRangecf(_zMin, _zMax, oz + dz * t0))
        t = t0;
    else if (wr.occluded(t1) && InRangecf(_zMin, _zMax, oz + dz * t1))
        t = t1;
    else
        return false;

    wr.setT(t);
    res->position = r.cast(t);

    res->normal = vec3f(res->position.row<0, 1>(), 0_sg);

    res->uv = vec2f(PiTwoToZeroOne(atan2(res->normal[1], res->normal[0])),
                    Saturate(Ratio(_zMin, _zMax, res->position[2])));

    res->dpdu = vec3f(-res->position[1], res->position[0], 0_sg);
    res->dpdv = vec3f(0_sg, 0_sg, _zMax - _zMin);

    res->normal = MakeReversedOrient(r.getDirection(), res->normal);

    surface_helper::ResToWorldSpace(trans, res);
    return true;
}
