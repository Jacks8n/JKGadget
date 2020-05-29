#pragma once

#include "igiscene/aggregate.h"
#include "serialize.h"

namespace igi {
    class scene {
        std::pmr::vector<IMaterial *> _materials;

        std::pmr::vector<ISurface *> _surfaces;

        aggregate _aggregate;

        color3 _background;

      public:
        using allocator_type = typename aggregate::allocator_type;

        META_B(scene, rflite::ctor_a([](const serializer_t &ser, const allocator_type &alloc) {
                   assert(ser.HasMember("material"));
                   assert(ser.HasMember("surface"));
                   assert(ser.HasMember("entity"));

                   const serializer_t &mser = ser["material"];
                   assert(mser.IsArray());

                   const serializer_t &sser = ser["surface"];
                   assert(sser.IsArray());

                   const serializer_t &eser = ser["entity"];
                   assert(eser.IsArray());

                   color3 back = ser.HasMember("background")
                                     ? serialization::Deserialize<color3>(ser["background"])
                                     : palette::black;
                   
                   std::pmr::vector<IMaterial *> mats = serialization::DeserializeArray<IMaterial *>(mser, alloc);
                   std::pmr::vector<ISurface *> surfs = serialization::DeserializeArray<ISurface *>(sser, alloc);
                   std::pmr::vector<entity> ents      = serialization::DeserializeArray<entity>(eser, alloc, mats, surfs);
                   return scene(std::move(mats), std::move(surfs), std::move(ents), std::move(back), alloc);
               }))
        META_E

        scene(std::pmr::vector<IMaterial *> mats, std::pmr::vector<ISurface *> surfs,
              std::pmr::vector<entity> entities, const color3 &background,
              const allocator_type &alloc)
            : _materials(std::move(mats)), _surfaces(std::move(surfs)),
              _aggregate(std::move(entities), alloc), _background(background) { }

        const aggregate &getAggregate() const {
            return _aggregate;
        }

        color3 getBackground() const {
            return _background;
        }
    };
}  // namespace igi
