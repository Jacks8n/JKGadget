#include "igigeometry/cylinder.h"

bool igi::cylinder::isHit(const ray &wr, const transform &trans) const {
    ray r = surface_helper::ToLocalRay(wr, trans);

    single zmin = r.getOrigin()[2], zmax = r.getEndpoint()[2];
    if (Lesscf(zmax, zmin))
        std::swap(zmax, zmin);
    if (!Overlapcf(zmin, zmax, _zMin, _zMax))
        return false;

    vec2f oxy = static_cast<vec2f>(r.getOrigin());
    vec2f dxy = static_cast<vec2f>(r.getDirection());
    single p  = -Dot(oxy, dxy);
    return Lesscf((oxy + dxy * p).magnitudeSqr(), _r * _r);
}

bool igi::cylinder::tryHit(ray &wr, const transform &trans, surface_interaction *res) const {
    const ray r = surface_helper::ToLocalRay(wr, trans);

    vec2f oxy(r.getOrigin());
    vec2f dxy(r.getDirection());

    single a = Dot(dxy, dxy);
    single b = Dot(oxy, dxy) * 2_sg;
    single c = Dot(oxy, oxy) - _r * _r;
    single d = b * b - 4_sg * a * c;

    if (!IsPoscf(d)) return false;

    a = .5_sg / a;
    b = -b * a;
    d = sqrt(d) * a;

    single oz = r.getOrigin()[2];
    single dz = r.getDirection()[2];
    single t;

    if (InRangecf(r.getTMin(), r.getT(), b - d)
        && InRangecf(_zMin, _zMax, oz + dz * (b - d)))
        t = b - d;
    else if (InRangecf(r.getTMin(), r.getT(), b + d)
             && InRangecf(_zMin, _zMax, oz + dz * (b + d)))
        t = b + d;
    else
        return false;

    wr.setT(t);
    res->position = r.cast(t);

    res->normal = vec3f(res->position[0], res->position[1], 0_sg);

    res->dpdu = vec3f(-res->position[1], res->position[0], 0_sg);
    res->dpdv = vec3f(0_sg, 0_sg, _zMax - _zMin);

    res->uv = vec2f(Saturate(PiTwoToZeroOne(atan2(res->normal[1], res->normal[0]))),
                    Saturate(Ratio(_zMin, _zMax, res->position[2])));

    res->normal = MakeReversedOrient(r.getDirection(), res->normal);

    surface_helper::ResToWorldSpace(trans, res);
    return true;
}
