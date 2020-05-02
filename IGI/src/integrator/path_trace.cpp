#include "igiintegrator/path_trace.h"

igi::color_rgb igi::path_trace::integrate(ray &r) {
    interaction i;
    if (!_scene.getAggregate().tryHit(r, i))
        return _scene.getBackground();

    return Saturate(integrate_impl(r.getDirection(), i, _depth));
}

igi::color_rgb igi::path_trace::integrate_impl(const igi::vec3f &o, const igi::interaction &interaction, size_t depth) {
    if (!depth)
        return igi::palette_rgb::black;

    const igi::IMaterial &mat       = *interaction.material;
    const surface_interaction &surf = interaction.surface;

    mat3x3f tanCoord;
    tanCoord.setCol(0, surf.dpdu.normalized());
    tanCoord.setCol(1, surf.dpdv.normalized());
    tanCoord.setCol(2, surf.normal.normalized());

    scatter scat;
    igi::interaction ia;
    color_rgb integral = mat.getLuminance() * -Dot(o, surf.normal);
    for (size_t i = 0; i < _split; i++) {
        scat = mat.getScatter(o, tanCoord, _random);

        if (scat.pdf == AsSingle(0))
            break;

        ray r(surf.position, scat.direction);
        if (_scene.getAggregate().tryHit(r, ia))
            integral = integral
                       + Mul(integrate_impl(scat.direction, ia, depth - 1),
                             mat(scat.direction, o, surf.normal))
                             * scat.pdf;
    }

    return integral;
}
