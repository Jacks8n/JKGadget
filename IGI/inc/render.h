#pragma once

#include <chrono>
#include <memory_resource>
#include <ostream>
#include "igiacceleration/parallel.h"
#include "igicamera/camera.h"
#include "igiintegrator/IIntegrator.h"
#include "igimath/mcode.h"
#include "igitexture/texture.h"

namespace igi {
    template <typename TCamera, typename TIntegrator>
    void render(const scene &scene, TCamera &&camera, TIntegrator &&integrator,
                texture_rgb &res, size_t spp = 1, std::ostream *log = nullptr) {
        igiassert(spp > 0);

        const single w = res.getWidth(), h = res.getHeight();
        const single wInv = 1_sg / w, hInv = 1_sg / h, sppInv = 1_sg / spp;
        parallel_context parallel([&]() {
            return std::make_tuple(integrator_context(),
                                   uniform_quad_distribution(vec2f::One(0_sg), vec2f(wInv, hInv)),
                                   std::ref(camera), std::ref(integrator), std::ref(scene), spp, sppInv);
        });
        auto job = parallel.schedule([](auto &context, vec2f uv, color3 *res) {
            auto &[ic, uqd, camera, integrator, scene, spp, sppInv] = context;

            ray ray;
            single p;
            vec2f sample;

            for (size_t i = 0; i < spp; i++) {
                sample = uqd(ic.pcg, &p) + uv;
                ray    = camera.getRay(sample);
                *res += (integrator.integrate(scene, ray, ic) / p) * sppInv;
            }
        });

        const size_t total  = res.getHeight() * res.getWidth() * spp;
        const size_t oneper = total / 100, modulo = total % 100;
        size_t issued = 1, percent = 0, residue = 0;

        static constexpr size_t BlockSize     = 16;
        static constexpr size_t PixelPerBlock = BlockSize * BlockSize;

        res.clear(palette::black);
        const auto start = std::chrono::high_resolution_clock::now();
        auto issue       = [&](vec2u uv) {
            job.issue(vec2f(uv), &res.at(uv[0], uv[1]));

            if (log && ++issued > oneper) {
                const auto elapsed = std::chrono::high_resolution_clock::now() - start;
                const auto ns      = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed);

                *log << ++percent << "%\t" << ns.count() * 1e-9 << "s\n";
                issued = (residue += modulo) > 100 ? (residue -= 100, 0) : 1;
            }
        };
        for (size_t v = 0, u = 0; v < h; u = 0) {
            if (v + BlockSize < h)
                for (; u + BlockSize < w; u += BlockSize) {
                    mvec<2, unsigned> morton;
                    for (size_t i = 0; i < PixelPerBlock; i++, ++morton)
                        issue(vec2u(u, v) + morton.coord());
                }

            const size_t bottom = v + BlockSize < h ? v + BlockSize : h;
            for (; v < bottom; v++)
                for (size_t i = u; i < w; i++)
                    issue(vec2u(i, v));
        }
        job.finish();
    }
}  // namespace igi
