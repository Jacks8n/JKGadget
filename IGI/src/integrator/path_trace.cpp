#include "igiintegrator/path_trace.h"

igi::color_rgb igi::path_trace::integrate(ray &r) {
    interaction i;
    if (!_scene.getAggregate().tryHit(r, i))
        return _scene.getBackground();

    return Saturate(integrate_impl(r.getDirection(), i, _depth));
}

igi::color_rgb igi::path_trace::integrate_impl(const igi::vec3f &o, const igi::interaction &interaction, size_t depth) {
    static std::uniform_real_distribution<single> urd;

    if (!depth)
        return igi::palette_rgb::black;

    const igi::IMaterial &mat       = *interaction.material;
    const surface_interaction &surf = interaction.surface;

    mat3x3f tanCoord;
    tanCoord.setCol(0, surf.dpdu.normalized());
    tanCoord.setCol(1, surf.dpdv.normalized());
    tanCoord.setCol(2, surf.normal.normalized());

    ray r;
    color_rgb bxdf;
    scatter scat;
    igi::interaction ia;
    color_rgb integral = mat.getLuminance() * -Dot(o, surf.normal);
    for (size_t i = 0; i < _split; i++) {
        scat = mat.getScatter(o, tanCoord, _random);
        if (scat.pdf == AsSingle(0) || (Lesscf(scat.pdf, .001) && urd(_random) < AsSingle(.5)))
            continue;

        bxdf = mat(scat.direction, o, surf.normal);
        if (Lesscf(bxdf.magnitudeSqr(), .001) && urd(_random) < AsSingle(.5))
            continue;

        r.reset(surf.position, scat.direction);
        if (_scene.getAggregate().tryHit(r, ia))
            integral = integral
                       + Mul(integrate_impl(scat.direction, ia, depth - 1), bxdf)
                             * scat.pdf;
    }

    return integral;
}
