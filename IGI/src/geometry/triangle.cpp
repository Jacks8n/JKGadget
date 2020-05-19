#include "igigeometry/triangle.h"

using namespace igi;

bool triangle::tryHit(ray &r, const transform &, surface_interaction *res) const {
    auto [a, b, c] = getPos();

    vec3f ra = r.toRaySpace(a);
    vec3f rb = r.toRaySpace(b);
    vec3f rc = r.toRaySpace(c);

    single tab = Cross(ra - rb, ra).magnitude();
    single tbc = Cross(rb - rc, rb).magnitude();
    single tca = Cross(rc - ra, rc).magnitude();
    if ((IsNegcf(tab) || IsNegcf(tbc) || IsNegcf(tca))
        && (IsPoscf(tab) || IsPoscf(tbc) || IsPoscf(tca)))
        return false;

    single det = tab + tbc + tca;
    if (Equalcf(det, 0))
        return false;

    single zsum = ra[2] * tab + rb[2] * tbc + rc[2] * tca;
    single tmin = r.getTMin() * det, tmax = r.getT() * det;
    if ((IsPoscf(det) && !InRangecf(tmin, tmax, zsum))
        || (IsNegcf(det) && !InRangecf(tmin, tmax, zsum)))
        return false;

    single detInv = 1_sg / det;
    single u      = tab * detInv;
    single v      = tbc * detInv;
    single w      = tca * detInv;

    r.setT(zsum * detInv);
    res->position = a * u + b * v + c * w;

    auto [uv0, uv1, uv2] = getUV();

    res->uv = uv0 * u + uv1 * v + uv2 * w;

    // (...).rows() : Returns a tuple encapsulating two vec3f's
    //   |
    //   |- (...).inverse() * (...) : Multiply matrix and a vector of vector
    //        |                 |- igivec(b - a, c - a) : Construct a vector of vector
    //        |
    //        |- mat2x2f(uv1 - uv0, uv2 - uv0) : Construct matrix from two column vectors
    std::tie(res->dpdu, res->dpdv) = (mat2x2f(uv1 - uv0, uv2 - uv0).inverse() * igivec(b - a, c - a)).rows();

    res->normal = Cross(res->dpdu, res->dpdv).normalized();
    return true;
}