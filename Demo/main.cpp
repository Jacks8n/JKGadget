#include "main.h"
#include "igigeometry/cylinder.h"
#include "igigeometry/sphere.h"
#include "igiintegrator/path_trace.h"
#include "igimaterial/material_emissive.h"
#include "igimaterial/material_phong.h"
#include "serialize.h"

using namespace demo;

int main() {
    //// A sphere with radius being 1000
    //igi::sphere sp(1000);
    //// A cylinder with radius being 0.35 and z ranging from -0.35 to 0.35
    //igi::cylinder cy(.35, -.35, .35);

    //// White emissive material with luminance being 35 nits
    //igi::material_emissive m0(35);
    //// An entity representing an emissive sphere
    //igi::entity e0(&sp, &m0);
    //// Move e0 to (-600, 600, -600)
    //e0.getTransform().translation(igi::vec3f(-600, 600, -600));

    //// Default phong material
    //igi::material_phong m1;
    //// An entity representing a cylinder with phong material
    //igi::entity e1(&cy, &m1);
    //// Rotate and move e to (0, 0, 2)
    //e1.getTransform()
    //    .rotation(igi::vec3f(igi::ToRad(45), igi::ToRad(45), igi::ToRad(45)))
    //    .translation(igi::vec3f(0, 0, 2));

    igi::mem_arena arena;
    std::pmr::polymorphic_allocator<char> alloc(&arena);

    std::ifstream fs("demo.json");

    fs.seekg(0, std::ios_base::end);
    size_t nbuf = fs.tellg();
    fs.seekg(0);

    char *config                   = alloc.allocate(nbuf);
    config[ReadConfig(fs, config)] = '\0';
    fs.close();

    std::cout << config;

    rapidjson::Document doc;
    doc.ParseInsitu(config);

    const auto &camProp   = doc["camera"];
    igi::camera_base *cam = igi::serialization::DeserializePmr<igi::camera_base>(camProp, alloc, camProp["type"].GetString());

    igi::scene *demo     = igi::serialization::Deserialize<igi::scene>(doc, alloc);
    igi::path_trace pt   = igi::serialization::Deserialize<igi::path_trace>(doc["integrator"]);
    igi::texture_rgb res = igi::serialization::Deserialize<igi::texture_rgb>(doc["film"], alloc);

    //run<1024, igi::path_trace>(  // sample 1024 times per pixel, using path_tracing
    //    "demo.png",              // output to demo.png
    //    512, 512,                // size of result image is 512 * 512
    //    { e0, e1 },              // entities to be rendered
    //    4, 4);                   // reflect 4 times, split to 4 rays at each reflection

    return 0;
}
