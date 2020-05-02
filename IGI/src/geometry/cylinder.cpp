#include "igigeometry/cylinder.h"

bool igi::cylinder::isHit(const ray& r) const {
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

bool igi::cylinder::tryHit(ray& r, surface_interaction* res) const {
    vec2f oxy = static_cast<vec2f>(r.getOrigin());
    vec2f dxy = static_cast<vec2f>(r.getDirection());

    single a = Dot(dxy, dxy);
    single b = Dot(oxy, dxy) * AsSingle(2);
    single c = Dot(oxy, oxy) - _r * _r;
    single d = b * b - AsSingle(4) * a * c;

    if (!IsPositivecf(d)) return false;

    a = AsSingle(.5) / a;
    b = -b * a;
    d = sqrt(d) * a;

    single oz = r.getOrigin()[2];
    single dz = r.getDirection()[2];

    if (InRangecf(0, r.getT(), b - d) && InRangecf(_zMin, _zMax, oz + dz * (b - d)))
        r.setT(b - d);
    else if (InRangecf(0, r.getT(), b + d) && InRangecf(_zMin, _zMax, oz + dz * (b + d)))
        r.setT(b + d);
    else
        return false;

    res->normal = res->position = r.getEndpoint();

    res->normal[2] = 0;
    res->normal *= AsSingle(1) / _r;
    if (IsPositivecf(Dot(res->normal, r.getDirection())))
        res->normal = -res->normal;

    res->dpdu = vec3f(-res->normal[1], res->normal[0], 0);
    res->dpdv = vec3f(0, 0, _zMax - _zMin);

    res->uv = vec2f(Saturate(PiTwoToZeroOne(atan2(res->normal[1], res->normal[0]))),
                    Saturate(Ratio(_zMin, _zMax, res->position[2])));
    return true;
}
