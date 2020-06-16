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

        META_BE(scene, rflite::func_a([](const serializer_t &ser, const allocator_type &alloc) {
                   const serializer_t &mser = ser["material"];
                   const serializer_t &sser = ser["surface"];
                   const serializer_t &eser = ser["entity"];

                   color3 back = ser.HasMember("background")
                                     ? serialization::Deserialize<color3>(ser["background"])
                                     : palette::black;

                   constexpr auto policy              = [](const serializer_t &ser) { return ser["type"].GetString(); };
                   std::pmr::vector<IMaterial *> mats = serialization::DeserializePmrArray<IMaterial, std::pmr::vector>(mser, alloc, policy);
                   std::pmr::vector<ISurface *> surfs = serialization::DeserializePmrArray<ISurface, std::pmr::vector>(sser, alloc, policy);
                   std::pmr::vector<entity> ents      = serialization::DeserializeArray<entity, std::pmr::vector>(eser, alloc, mats, surfs);
                   return rflite::meta_helper::any_new<scene>(std::move(mats), std::move(surfs), std::move(ents), back, alloc);
               }))

        scene(std::pmr::vector<IMaterial *> mats, std::pmr::vector<ISurface *> surfs,
              std::pmr::vector<entity> entities, const color3 &background,
              const allocator_type &alloc)
            : _materials(std::move(mats), alloc), _surfaces(std::move(surfs), alloc),
              _aggregate(std::move(entities), alloc), _background(background) { }

        const aggregate &getAggregate() const {
            return _aggregate;
        }

        color3 getBackground() const {
            return _background;
        }
    };
}  // namespace igi
