#include "igiintegrator/path_trace.h"

igi::color_rgb igi::path_trace::integrate(ray &r) {
    interaction i;
    if (!_scene.getAggregate().tryHit(r, i))
        return _scene.getBackground();

    return integrate_impl(r.getDirection(), i, _depth);
}

igi::color_rgb igi::path_trace::integrate_impl(const igi::vec3f &o, const igi::interaction &interaction, size_t depth) {
    static std::uniform_real_distribution<single> urd;

    const surface_interaction &surf = interaction.surface;
    const igi::IMaterial &mat       = *interaction.material;

    color_rgb lu = mat.getLuminance() * -Dot(o, surf.normal);

    if (!depth) 
        return lu;

    mat3x3f tanCoord;
    tanCoord.setCol(0, surf.dpdu.normalized());
    tanCoord.setCol(1, surf.dpdv.normalized());
    tanCoord.setCol(2, surf.normal.normalized());

    ray r;
    scatter scat;
    color_rgb bxdf;
    igi::interaction ia;

    single pint    = 0;
    color_rgb lint = palette_rgb::black;
    for (size_t i = 0; i < _split; i++) {
        scat = mat.getScatter(o, tanCoord, _random);
        if (scat.pdf == AsSingle(0) || (Lesscf(scat.pdf, .001) && urd(_random) < AsSingle(.5)))
            continue;

        bxdf = mat(scat.direction, o, surf.normal);
        if (Lesscf(bxdf.magnitudeSqr(), .001) && urd(_random) < AsSingle(.5))
            continue;

        pint += scat.pdf;
        r.reset(surf.position, scat.direction);
        if (_scene.getAggregate().tryHit(r, ia))
            lint = lint + Mul(integrate_impl(scat.direction, ia, depth - 1), bxdf) * scat.pdf;
    }

    if (Equalcf(pint, 0))
        return lu;

    return lu + lint * static_cast<color_channel_t>(AsSingle(1) / pint);
}
