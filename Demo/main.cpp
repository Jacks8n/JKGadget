#include <fstream>
#include "igicamera/camera.h"
#include "igigeometry/cylinder.h"
#include "igigeometry/sphere.h"
#include "igiintegrator/path_trace.h"
#include "igimaterial/material_emissive.h"
#include "igimaterial/material_phong.h"
#include "igiscene/aggregate_sbvh.h"
#include "igiscene/scene.h"
#include "render.h"

int main() {
    igi::mem_arena arena(1024 * 1024 * 10);

    // A sphere with radius being 1000
    igi::sphere sp(1000);
    // A cylinder with radius being 0.3 and z ranging from -0.3 to 0.3
    igi::cylinder cy(.3, -.3, .3);

    // White emissive material with luminance being 35 nits
    igi::material_emissive m0(35);
    // An entity representing an emissive sphere
    igi::entity e0(sp, m0);
    // Move e0 to (-600, 600, -600)
    e0.getTransform().translation(igi::vec3f(-600, 600, -600));

    // Default phong material
    igi::material_phong m1;
    // An entity representing a cylinder with phong material
    igi::entity e1(cy, m1);
    // Rotate and move e to (0, 0, 2)
    e1.getTransform()
        .rotation(igi::vec3f(igi::ToRad(45), igi::ToRad(45), igi::ToRad(45)))
        .translation(igi::vec3f(0, 0, 2));

    // Simplest aggregate of entities
    igi::aggregate_sbvh av({ e0, e1 }, &arena);
    // Scene with black background
    igi::scene s(av);
    // Integrator to use
    // Reflect 4 times
    // Split into 4 rays at every reflection
    igi::path_trace pt(s, 4, 4);

    constexpr size_t w = 512, h = 512;

    // Camera to observe the scene
    // igi::camera_orthographic c(2, 2);
    igi::camera_perspective c(70, igi::AsSingle(w) / h);

    // Result texture
    igi::texture_rgb t(w, h, &arena);
    render(c, t, pt, arena, 1024);

    std::ofstream o("demo.png", std::ios_base::binary);
    pngparvus::png_writer().write(o, t);
    return 0;
}
