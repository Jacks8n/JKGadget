#pragma once

#include <memory_resource>
#include "igiacceleration/task_queue.h"
#include "igicamera/camera.h"
#include "igiintegrator/IIntegrator.h"
#include "igitexture/texture.h"

namespace igi {
    template <typename TCam, typename TInt>
    inline void render(const TCam &camera, texture_rgb &res, const TInt &integrator,
                       mem_arena &arena, size_t spp = 1) {
        struct task {
            size_t u, v;
            color_rgb *res;

            task()        = default;
            task(task &&) = default;
            task(size_t u, size_t v, color_rgb *res)
                : u(u), v(v), res(res) {
            }
        };

        struct context {
            integrator_context ic;
            unit_square_distribution usd;

            context(const typename integrator_context::allocator_type &alloc)
                : ic(alloc), usd() { }
        };

        res.clear(palette_rgb::black);

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

                ray r       = camera.getRay(xy[0] / w, xy[1] / h);
                color_rgb i = integrator.integrate(r, c.ic) * p;

                std::scoped_lock sl(m);
                *in.res = *in.res + i;
            },
            1024, alloc, alloc);

        for (size_t j = 0; j < res.getHeight(); j++)
            for (size_t i = 0; i < res.getWidth(); i++)
                for (size_t k = 0; k < spp; k++)
                    group->issue(i, j, &res.get(i, j));

        group->waitFinish();

        single sppinv = 1_sg / spp;
        for (size_t j = 0; j < res.getHeight(); j++)
            for (size_t i = 0; i < res.getWidth(); i++) {
                color_rgb &c = res.get(i, j);
                c            = c * sppinv;
            }
    }
}  // namespace igi
