#include "igigeometry/triangle.h"

bool igi::triangle::isHit(const ray &r, const transform &trans) const {
    const auto &[a, b, c] = getPos();

    vec3f ra = r.toRaySpace(a);
    vec3f rb = r.toRaySpace(b);
    vec3f rc = r.toRaySpace(c);

    vec2f ra2(ra);
    vec2f rb2(rb);
    vec2f rc2(rc);

    vec2f e0 = rb2 - ra2;
    vec2f e1 = rc2 - rb2;
    vec2f e2 = ra2 - rc2;

    esingle a01 = Cross(e0, ra2);
    esingle a12 = Cross(e1, rb2);
    esingle a20 = Cross(e2, rc2);

    if (!((a01 > 0_sg && a12 > 0_sg && a20 > 0_sg)
          || (a01 < 0_sg && a12 < 0_sg && a20 < 0_sg)))
        return false;

    esingle det = a01 + a12 + a20;
    if (det == 0_sg)
        return false;

    esingle zsum = ra[2] * a12 + rb[2] * a20 + rc[2] * a01;
    esingle tmin = r.getTMin() * det, tmax = r.getT() * det;
    if ((det > 0_sg && !InRangecf(tmin, tmax, zsum))
        || (det < 0_sg && !InRangecf(tmax, tmin, zsum)))
        return false;

    esingle detInv = 1_sg / det;
    zsum *= detInv;
    if (!InRangecf(r.getTMin(), r.getT(), zsum))
        return false;

    return true;
}

bool igi::triangle::tryHit(ray &r, const transform &trans, surface_interaction *res) const {
    // solve: <n, v - (o + td)> = 0
    // <n, v> - <n, o> - t<n, d> = 0
    // o = (0, 0, 0), d = (0, 0, 1), since triangles are projected to ray space
    // t = <n, v> / n.z
    //
    // e_i := v_{(i + 1) % 3} - v_i
    // n = cross(e_0, e_1)
    // t = det{ e_0, e_1, v } / cross(e_0.xy, e_1.xy)

    const auto &[a, b, c] = getPos();

    vec3f ra = r.toRaySpace(a);
    vec3f rb = r.toRaySpace(b);
    vec3f rc = r.toRaySpace(c);

    vec2f ra2(ra);
    vec2f rb2(rb);
    vec2f rc2(rc);

    vec2f e0 = rb2 - ra2;
    vec2f e1 = rc2 - rb2;
    vec2f e2 = ra2 - rc2;

    esingle a01 = Cross(e0, ra2);
    esingle a12 = Cross(e1, rb2);
    esingle a20 = Cross(e2, rc2);

    if (!((a01 > 0_sg && a12 > 0_sg && a20 > 0_sg)
          || (a01 < 0_sg && a12 < 0_sg && a20 < 0_sg)))
        return false;

    esingle det = a01 + a12 + a20;
    if (det == 0_sg)
        return false;

    esingle zsum = ra[2] * a12 + rb[2] * a20 + rc[2] * a01;
    esingle tmin = r.getTMin() * det, tmax = r.getT() * det;
    if ((det > 0_sg && !InRangecf(tmin, tmax, zsum))
        || (det < 0_sg && !InRangecf(tmax, tmin, zsum)))
        return false;

    esingle detInv = 1_sg / det;
    zsum *= detInv;
    if (!InRangecf(r.getTMin(), r.getT(), zsum))
        return false;

    single u = a01 * detInv;
    single v = a12 * detInv;
    single w = a20 * detInv;

    r.setT(zsum);
    res->position = vec3f(a * u + b * v + c * w);

    auto [uv0, uv1, uv2] = getUV();

    res->uv = vec2f(uv0 * u + uv1 * v + uv2 * w);

    // (...).rows() : Returns a tuple encapsulating two vec3f's
    //   |
    //   |- (...).inverse() * (...) : Multiply matrix and a vector of vector
    //        |                 |- igivec(b - a, c - a) : Construct a vector of vector
    //        |
    //        |- mat2x2f(uv1 - uv0, uv2 - uv0) : Construct matrix from two column vectors
    std::tie(res->dpdu, res->dpdv) = mat2x2f(uv1 - uv0, uv2 - uv0).inverse().operator*(igivec(b - a, c - a)).row();

    res->normal = Cross(res->dpdu, res->dpdv).normalized();
    return true;
}
