#include "main.h"
#include "igigeometry/cylinder.h"
#include "igigeometry/sphere.h"
#include "igigeometry/triangle.h"
#include "igiintegrator/integrator_debug.h"
#include "igiintegrator/path_trace.h"
#include "igimaterial/material_emissive.h"
#include "igimaterial/material_phong.h"

using namespace demo;

using path_with_texture = std::pair<std::string_view, igi::texture_rgb>;

path_with_texture demo_json(std::pmr::polymorphic_allocator<char> &alloc);
path_with_texture demo_primitives(std::pmr::polymorphic_allocator<char> &alloc);

int main() {
    igi::mem_arena arena;
    std::pmr::polymorphic_allocator<char> alloc(&arena);
    igi::context::ExternalAllocator = &alloc;

    //path_with_texture res = demo_json(alloc);
    path_with_texture res = demo_primitives(alloc);

    std::ofstream os;
    os.open(res.first.data(), std::ios_base::binary);

    if (os.fail()) {
        std::cerr << "failed to start writing output" << std::endl;
        return -1;
    }

    pngparvus::png_writer writer;
    writer.write(os, res.second);

    os.close();

    return 0;
}

path_with_texture demo_json(std::pmr::polymorphic_allocator<char> &alloc) {
    char *config = ReadConfig("demo.json", alloc);

    rapidjson::Document doc;
    doc.ParseInsitu(config);

    const auto &camProp   = doc["camera"];
    igi::camera_base *cam = igi::serialization::DeserializePmr<igi::camera_base>(camProp, camProp["type"].GetString());

    igi::scene *demo     = igi::serialization::Deserialize<igi::scene>(doc);
    igi::path_trace pt   = igi::serialization::Deserialize<igi::path_trace>(doc["integrator"]);
    igi::texture_rgb res = igi::serialization::Deserialize<igi::texture_rgb>(doc["film"]);

    IGI_SERIALIZE_OPTIONAL(size_t, spp, 4, doc);

    std::string_view path = igi::serialization::Deserialize<std::string_view>(doc["output"]["path"]);

    std::cout << "film size: " << res.getWidth() << " * " << res.getHeight() << '\n';
    if (!has_spp)
        std::cout << "spp not set, using default value\n";
    std::cout << "spp: " << spp << '\n';

    igi::render(*demo, *cam, pt, res, spp, &std::cout);

    return std::make_pair(path, res);
}

path_with_texture demo_primitives(std::pmr::polymorphic_allocator<char> &alloc) {
    static constexpr size_t nentity = 12;

    static igi::vec3f vertices[3 * nentity];

    std::default_random_engine e(reinterpret_cast<size_t>(&alloc));
    std::uniform_real_distribution<igi::single> rd(-.5f, .5f);

    for (auto &i : vertices)
        i = igi::vec3f(rd(e), rd(e), rd(e) + 5);

    igi::triangle_mesh mesh;
    mesh.setPos(std::begin(vertices), std::end(vertices));
    mesh.setTriangle(igi::triangle_topology::list);

    igi::material_phong mat0;
    igi::material_emissive mat1;
    igi::shared_vector<igi::IMaterial *> mats(alloc, &mat0, &mat1);

    igi::shared_vector<igi::ISurface *> surfs(nentity, alloc);
    mesh.addTrianglesTo(surfs.begin());

    igi::shared_vector<igi::entity> entities(nentity, alloc);
    for (size_t i = 0; i < nentity; i++)
        new (&entities[i]) igi::entity(surfs[i], mats[i & 1]);

    igi::scene demo(mats.as_shared_ptr(), surfs.as_shared_ptr(), entities.as_shared_ptr(), nentity, igi::palette::black);

    igi::camera_perspective cam;
    cam.setFar(7.f);

    igi::path_trace itg(4, 4);
    //igi::integrator_debug itg(igi::integrator_debug_mode::depth);

    igi::texture_rgb res(256, 256);

    igi::render(demo, cam, itg, res, 1, &std::cout);

    return std::make_pair("demo_primitives.png", res);
}
