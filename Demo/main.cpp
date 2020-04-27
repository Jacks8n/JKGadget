#include <fstream>
#include "igigeometry/sphere.h"
#include "igimaterial/material_phong.h"
#include "igiscene/aggregate_vector.h"
#include "igiscene/scene.h"
#include "igicamera/camera.h"
#include "igiintegrator/path_trace.h"

int main() {
    igi::sphere b(.5);
    igi::material_phong m;
    igi::entity e(b, m);
    e.getTransform().translation(igi::vec3f(.5, 0, 2));

    igi::aggregate_vector av{ &e };
    igi::scene s(av);
    igi::path_trace pt(s);

    igi::camera_orthographic c(2, 2);
    igi::texture_rgb t(512, 512);
    c.render(t, pt);

    std::ofstream o("demo.png", std::ios_base::binary);
    pngparvus::png_writer().write(o, t);
    return 0;
}
