#include "igiintegrator/path_trace.h"

igi::color3 igi::path_trace::integrate_impl(const scene &scene, const vec3f &o, const interaction &interaction,
                                            size_t depth, integrator_context &context) const {
    std::uniform_real_distribution<single> urd;

    const surface_interaction &surf = interaction.surface;
    const igi::IMaterial &mat       = *interaction.material;

    color3 lu = mat.getLuminance() * -Dot(o, surf.normal);

    if (!depth)
        return lu;

    mat3x3f ns = surf.getNormalSpace();

    ray r;
    scatter scat;
    color3 bxdf;
    igi::interaction ia;

    single pint = 0;
    color3 lint = palette::black;
    for (size_t i = 0; i < _split; i++) {
        scat = mat.getScatter(o, ns, context.pcg);
        if ((scat.pdf == 0_sg) || (scat.pdf < .001_sg && urd(context.pcg) < .5_sg))
            continue;

        bxdf = mat(o, scat.direction, surf.normal);

        single roulette = bxdf.brightness();
        if (roulette == 0_sg || (roulette < .005_sg && urd(context.pcg) < .5_sg))
            continue;

        pint += scat.pdf;
        r.reset(surf.position, scat.direction);
        if (scene.getAggregate().tryHit(r, &ia, context.itrtmp))
            lint = lint + (integrate_impl(scene, scat.direction, ia, depth - 1, context) * bxdf) * scat.pdf;
    }

    if (pint == 0_sg)
        return lu;

    return lu + lint * (1_col / pint);
}
