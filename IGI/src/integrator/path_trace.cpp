#include "igiintegrator/path_trace.h"

igi::color_rgb igi::path_trace::integrate_impl(
    const igi::vec3f &o, const igi::interaction &interaction, size_t depth, random_engine_t &rand) const {
    std::uniform_real_distribution<single> urd;

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
        scat = mat.getScatter(o, tanCoord, rand);
        if ((scat.pdf == 0_sg) || (Lesscf(scat.pdf, .001) && urd(rand) < .5_sg))
            continue;

        bxdf = mat(scat.direction, o, surf.normal);
        if (Lesscf(bxdf.magnitudeSqr(), .001) && urd(rand) < .5_sg)
            continue;

        pint += scat.pdf;
        r.reset(surf.position, scat.direction);
        if (_scene.getAggregate().tryHit(r, &ia))
            lint = lint + Mul(integrate_impl(scat.direction, ia, depth - 1, rand), bxdf) * scat.pdf;
    }

    if (Equalcf(pint, 0))
        return lu;

    return lu + lint * static_cast<color_channel_t>(1) / AsSingle(pint);
}
