#include "main.h"
#include "igigeometry/cylinder.h"
#include "igigeometry/sphere.h"
#include "igiintegrator/path_trace.h"
#include "igimaterial/material_emissive.h"
#include "igimaterial/material_phong.h"

using namespace demo;

int main() {
    igi::mem_arena arena;
    std::pmr::polymorphic_allocator<char> alloc(&arena);

    char *config = ReadConfig("demo.json", alloc);

    rapidjson::Document doc;
    doc.ParseInsitu(config);

    const auto &camProp   = doc["camera"];
    igi::camera_base *cam = igi::serialization::DeserializePmr<igi::camera_base>(camProp, alloc, camProp["type"].GetString());

    igi::scene *demo     = igi::serialization::Deserialize<igi::scene>(doc, alloc);
    igi::path_trace pt   = igi::serialization::Deserialize<igi::path_trace>(doc["integrator"]);
    igi::texture_rgb res = igi::serialization::Deserialize<igi::texture_rgb>(doc["film"], alloc);

    IGI_SERIALIZE_OPTIONAL(size_t, spp, 4, doc);

    std::string_view path = igi::serialization::Deserialize<std::string_view>(doc["output"]["path"]);

    std::cout << "film size: " << res.getWidth() << " * " << res.getHeight() << '\n';
    if (!has_spp)
        std::cout << "spp not set, using default value\n";
    std::cout << "spp: " << spp << '\n';

    igi::render(*demo, *cam, pt, arena, res, spp, &std::cout);

    std::ofstream os;
    os.open(path.data(), std::ios_base::binary);

    if (os.fail()) {
        std::cerr << "failed to start writing output" << std::endl;
        return -1;
    }

    pngparvus::png_writer writer;
    writer.write(os, res);

    os.close();

    return 0;
}
