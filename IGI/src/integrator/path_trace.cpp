#include "igiintegrator/path_trace.h"

igi::color3 igi::path_trace::integrate_impl(const scene &scene, const vec3f &o, const interaction &interaction,
                                            size_t depth, integrator_context &context) const {
    std::uniform_real_distribution<single> urd;

    const surface_interaction &surf = interaction.surface;
    const igi::IMaterial &mat       = *interaction.material;

    const color3 lu = mat.getLuminance() * -Dot(o, surf.normal);

    if (!depth)
        return lu;

    const mat3x3f ns = surf.getNormalSpace();

    ray r;
    scatter scat;
    color3 bxdf;
    igi::interaction ia;

    int nsamp   = 0;
    color3 lint = palette::black;
    for (size_t i = 0; i < _split; i++) {
        scat = mat.getScatter(o, ns, context.pcg);
        if (scat.pdf < .001_sg)
            continue;

        bxdf = mat(o, scat.direction, surf.normal);

        if (bxdf.brightness() < .005_sg) {
            if (urd(context.pcg) < .5_sg)
                continue;
            scat.pdf *= .5_sg;
        }

        r = ray(surf.position, scat.direction);
        if (scene.getAggregate().tryHit(r, &ia, context.itrtmp)) {
            single weight = 1_sg / r.getT();
            weight        = weight * weight / scat.pdf;

            lint = lint + integrate_impl(scene, scat.direction, ia, depth - 1, context) * bxdf * weight;
        }

        nsamp++;
    }

    return nsamp ? lu + lint / nsamp : lu;
}
