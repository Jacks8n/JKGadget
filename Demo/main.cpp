#include <fstream>
#include "igicamera/camera.h"
#include "igigeometry/sphere.h"
#include "igiintegrator/path_trace.h"
#include "igimaterial/material_phong.h"
#include "igiscene/aggregate_vector.h"
#include "igiscene/scene.h"

int main() {
    // A ball whose radius is 0.5
    igi::sphere b(.5);
    // Default phong material
    igi::material_phong m;
    // An entity representing a ball with phong material
    igi::entity e(b, m);
    // Move e to (0, 0, 2)
    e.getTransform().translation(igi::vec3f(0, 0, 2));

    // Simplest aggregate of entities
    igi::aggregate_vector av { &e };
    // Scene with black background
    igi::scene s(av);
    // Integrator to use
    igi::path_trace pt(s);

    // Camera to observe the scene
    igi::camera_orthographic c(2, 2);
    // Result texture
    igi::texture_rgb t(512, 512);
    c.render(t, pt);

    std::ofstream o("demo.png", std::ios_base::binary);
    pngparvus::png_writer().write(o, t);
    return 0;
}
