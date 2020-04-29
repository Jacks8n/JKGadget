#include <fstream>
#include "igicamera/camera.h"
#include "igigeometry/sphere.h"
#include "igiintegrator/path_trace.h"
#include "igimaterial/material_phong.h"
#include "igiscene/aggregate_vector.h"
#include "igiscene/scene.h"
#include "reflitest.h"

struct st {
    META_BEGIN(st)

    META(foo, 4.2)
    double foo;

    META(bar, "hello, world", "It's pointer")
    st* bar;

    META_GATHER
};

int main() {
    constexpr st sss { 2.71, nullptr };
    constexpr auto meta_foo   = st::meta_info_t::GetMetaInfo("foo");
    constexpr auto name_foo   = meta_foo.member_name();
    constexpr auto value_foo  = sss.*meta_foo.member_ptr();
    constexpr auto info_foo   = std::get<0>(meta_foo.value);
    constexpr auto meta_bar   = st::meta_info_t::GetMetaInfo("bar");
    constexpr auto value0_bar = std::get<0>(meta_bar.value);
    constexpr auto value1_bar = std::get<1>(meta_bar.value);

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
