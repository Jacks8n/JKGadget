#pragma once

#include <fstream>
#include <iostream>
#include "igicamera/camera.h"
#include "igiintegrator/path_trace.h"
#include "igiscene/aggregate.h"
#include "igiscene/scene.h"
#include "igiscene/scene_parser.h"
#include "render.h"

namespace demo {
    template <size_t SPP, typename TAlg, typename... TArgs>
    static void run(const char *path, size_t w, size_t h, typename igi::aggregate::initializer_list_t entities, TArgs &&... args) {
        igi::mem_arena arena(1024 * 1024 * 10);

        igi::aggregate a(entities, &arena);

        igi::scene s(a);

        TAlg alg(std::forward<TArgs>(args)...);

        igi::camera_perspective cam(70_sg, igi::AsSingle(w) / igi::AsSingle(h));

        igi::texture_rgb img(w, h, &arena);

        igi::render(cam, img, alg, arena, SPP, std::cout);

        std::ofstream o(path, std::ios_base::binary);
        pngparvus::png_writer().write(o, t);
    }
}  // namespace demo