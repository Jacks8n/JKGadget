﻿#include "igigeometry/cylinder.h"

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
    esingle t0, t1;
    if (!Quadratic(a, b, c, &t0, &t1))
        return false;

    single oz = r.getOrigin()[2];
    single dz = r.getDirection()[2];
    esingle t;

    if (r.isNearerT(t0) && InRangecf(_zMin, _zMax, oz + dz * t0))
        t = t0;
    else if (r.isNearerT(t1) && InRangecf(_zMin, _zMax, oz + dz * t1))
        t = t1;
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
