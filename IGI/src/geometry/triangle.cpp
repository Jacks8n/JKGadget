#include "igigeometry/triangle.h"

template <typename T, size_t Depth = 1>
bool tryHitTriangle(const igi::triangle &t, igi::ray &r, const igi::transform &trans, igi::surface_interaction *res);

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
    return tryHitTriangle<single>(*this, r, trans, res);
}

template <typename T, size_t Depth>
bool tryHitTriangle(const igi::triangle &t, igi::ray &r, const igi::transform &trans, igi::surface_interaction *res) {
    using precise = igi::precise_float_t<T>;
    using single  = T;
    using esingle = igi::error_single<T>;
    using vec2ef  = igi::vec2<esingle>;
    using vec3ef  = igi::vec3<esingle>;

    static constexpr bool TryPrecise = Depth > 0 && !std::is_same_v<T, precise>;

    static constexpr T Zero(0), One(1);

    auto preciseHit = [&]() {
        if constexpr (TryPrecise)
            return tryHitTriangle<precise, Depth - 1>(t, r, trans, res);
        else
            return false;
    };

    const auto &[a, b, c] = t.getPos();

    const vec3ef ra(r.toRaySpace(trans.mulPos(a)));
    const vec3ef rb(r.toRaySpace(trans.mulPos(b)));
    const vec3ef rc(r.toRaySpace(trans.mulPos(c)));

    const vec2ef ra2(ra);
    const vec2ef rb2(rb);
    const vec2ef rc2(rc);

    const vec2ef e0(rb2 - ra2);
    const vec2ef e1(rc2 - rb2);
    const vec2ef e2(ra2 - rc2);

    const esingle a01 = igi::Cross(e0, ra2);
    const esingle a12 = igi::Cross(e1, rb2);
    const esingle a20 = igi::Cross(e2, rc2);

    int cmp = 0;
    {
        const esingle as[3] { a01, a12, a20 };
        for (size_t i = 0; i < 3; i++) {
            if (as[i] > Zero)
                cmp += 3;
            else if (as[i] < Zero)
                cmp--;
        }
    }

    if (cmp != 9 && cmp != -3) {
        if constexpr (TryPrecise) {
            if (cmp != 1 && cmp != 2 && cmp != 5)
                preciseHit();
        }
        else
            return false;
    }

    const esingle det = a01 + a12 + a20;
    if (det == Zero)
        return preciseHit();

    esingle zsum = ra[2] * a12 + rb[2] * a20 + rc[2] * a01;
    esingle tmin = r.getTMin() * det, tmax = r.getT() * det;
    if (tmin > tmax)
        std::swap(tmin, tmax);
    if (!InRangecf(tmin, tmax, zsum))
        return preciseHit();

    const single detInv = One / det;

    const single u = a01 * detInv;
    const single v = a12 * detInv;
    const single w = a20 * detInv;

    r.setT(static_cast<igi::esingle>(zsum * detInv));
    res->position = igi::vec3f(a * u + b * v + c * w);

    auto [uv0, uv1, uv2] = t.getUV();

    res->uv = igi::vec2f(uv0 * u + uv1 * v + uv2 * w);

    // (...).rows() : Returns a tuple encapsulating two vec3f's
    //   |
    //   |- (...).inverse() * (...) : Multiply matrix and a vector of vector
    //        |                 |- igivec(b - a, c - a) : Construct a vector of vector
    //        |
    //        |- mat2x2f(uv1 - uv0, uv2 - uv0) : Construct matrix from two column vectors
    std::tie(res->dpdu, res->dpdv) = igi::mat2x2f(uv1 - uv0, uv2 - uv0).inverse().operator*(igivec(b - a, c - a)).row();

    res->normal = Cross(res->dpdu, res->dpdv).normalized();
    return true;
}
