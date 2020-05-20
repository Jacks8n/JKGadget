#pragma once

#include <chrono>
#include <memory_resource>
#include <ostream>
#include "igiacceleration/task_queue.h"
#include "igicamera/camera.h"
#include "igiintegrator/IIntegrator.h"
#include "igitexture/texture.h"

namespace igi {
    template <typename TCam, typename TInt>
    inline void render(const TCam &camera, texture_rgb &res, const TInt &integrator,
                       mem_arena &arena, size_t spp = 1, std::ostream *logos = nullptr) {
        struct task {
            size_t u, v;
            color3 *res;

            task()        = default;
            task(task &&) = default;
            task(size_t u, size_t v, color3 *res)
                : u(u), v(v), res(res) {
            }
        };

        struct context {
            integrator_context ic;
            unit_square_distribution usd;

            context(const typename integrator_context::allocator_type &alloc)
                : ic(alloc), usd() { }
        };

        res.clear(palette::black);

        std::mutex m;
        single w = res.getWidth(), h = res.getHeight();
        typename worker_group<task, context>::allocator_type alloc(&arena);
        auto group = worker_group<task, context>::DetachMax(
            [&, w, h](std::pair<task, context> &pair) {
                task &in(pair.first);
                context &c(pair.second);

                single p;
                vec2f xy = c.usd(c.ic.pcg, &p);
                xy[0] += static_cast<single>(in.u);
                xy[1] += static_cast<single>(in.v);

                ray r    = camera.getRay(xy[0] / w, xy[1] / h);
                color3 i = integrator.integrate(r, c.ic) * p;

                std::scoped_lock sl(m);
                *in.res = *in.res + i;
            },
            1024, alloc, alloc);

        size_t total  = res.getHeight() * res.getWidth() * spp;
        single oneper = total / 100_sg, oneperInv = 100_sg / total;
        size_t issued = 0_sg, totalIssued = 0_sg;
        size_t prec;
        if (logos) {
            prec = logos->precision();

            logos->precision(3);
            *logos << std::fixed;
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (size_t j = 0; j < res.getHeight(); j++)
            for (size_t i = 0; i < res.getWidth(); i++)
                for (size_t k = 0; k < spp; k++) {
                    group->issue(i, j, &res.get(i, j));

                    if (logos && ++issued >= oneper) {
                        auto elapsed = std::chrono::high_resolution_clock::now() - start;

                        totalIssued += issued;
                        *logos << oneperInv * totalIssued << "%\t"
                               << elapsed.count() * 1e-9 << "s used" << std::endl;
                        issued = 0;
                    }
                }

        group->waitFinish();

        if (logos) {
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            *logos << "finished in " << elapsed.count() * 1e-9 << "s" << std::endl;

            logos->precision(prec);
            logos << std::defaultfloat;
        }

        single sppinv = 1_sg / spp;
        for (size_t j = 0; j < res.getHeight(); j++)
            for (size_t i = 0; i < res.getWidth(); i++) {
                color3 &c = res.get(i, j);

                c = c * sppinv;
            }
    }
}  // namespace igi
