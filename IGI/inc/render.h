#pragma once

#include <chrono>
#include <memory_resource>
#include <ostream>
#include "igiacceleration/task_queue.h"
#include "igicamera/camera.h"
#include "igiintegrator/IIntegrator.h"
#include "igitexture/texture.h"

namespace igi {
    template <typename TCam>
    inline void render(const scene &scene, const TCam &camera, const IIntegrator &integrator,
                       texture_rgb &res, size_t spp = 1, std::ostream *log = nullptr) {
        struct task {
            size_t u, v;
            color3 *res;

            task()             = default;
            task(const task &) = default;

            task(size_t u, size_t v, color3 *res)
                : u(u), v(v), res(res) {
            }
        };

        struct context {
            integrator_context ic;
            unit_square_distribution usd;

            context() = default;
        };

        res.clear(palette::black);

        std::mutex m;
        single w = res.getWidth(), h = res.getHeight();
        auto group = worker_group<task, context>::DetachMax(
            [&, w, h](std::pair<task, context> &pair) {
                task &in(pair.first);
                context &c(pair.second);

                single p;
                vec2f xy = c.usd(c.ic.pcg, &p);
                xy[0] += static_cast<single>(in.u);
                xy[1] += static_cast<single>(in.v);

                ray r    = camera.getRay(xy[0] / w, xy[1] / h);
                color3 i = integrator.integrate(scene, r, c.ic) * p;

                std::scoped_lock sl(m);
                *in.res = *in.res + i;
            },
            1024);

        size_t total  = res.getHeight() * res.getWidth() * spp;
        single oneper = total / 100_sg, oneperInv = 100_sg / total;
        size_t issued = 0_sg, totalIssued = 0_sg;
        size_t prec;
        if (log) {
            prec = log->precision();

            log->precision(3);
            *log << std::fixed;
        }

        auto start = std::chrono::high_resolution_clock::now();
        for (size_t j = 0; j < res.getHeight(); j++)
            for (size_t i = 0; i < res.getWidth(); i++)
                for (size_t k = 0; k < spp; k++) {
                    group->issue(i, j, &res.get(i, j));

                    if (log && ++issued >= oneper) {
                        auto elapsed = std::chrono::high_resolution_clock::now() - start;

                        totalIssued += issued;
                        *log << oneperInv * totalIssued << "%\t"
                             << elapsed.count() * 1e-9 << "s used" << std::endl;
                        issued = 0;
                    }
                }

        group->waitFinish();

        single sppinv = 1_sg / spp;
        for (size_t j = 0; j < res.getHeight(); j++)
            for (size_t i = 0; i < res.getWidth(); i++) {
                color3 &c = res.get(i, j);

                c = c * sppinv;
            }

        if (log) {
            auto elapsed = std::chrono::high_resolution_clock::now() - start;
            *log << "finished in " << elapsed.count() * 1e-9 << "s" << std::endl;

            log->precision(prec);
            *log << std::defaultfloat;
        }
    }
}  // namespace igi
