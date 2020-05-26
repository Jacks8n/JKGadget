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
        META_B(scene, rflite::ctor([](const serializer_t &ser,
                                      const std::pmr::polymorphic_allocator<void *> &alloc) {
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

                   return scene(serialization::Deserialize<IMaterial *>(mser, alloc),
                                serialization::Deserialize<IMaterial *>(sser, alloc),
                                serialization::Deserialize<entity>(sser, alloc), alloc, back);
               }))
        META_E

        scene(std::pmr::vector<IMaterial *> &&mats, std::pmr::vector<ISurface *> &&surfs,
              const std::pmr::vector<entity> &entities, typename aggregate::allocator_type &alloc,
              color3 background = palette::black)
            : _materials(std::move(mats)), _surfaces(std::move(surfs)),
              _aggregate(entities, alloc), _background(background) { }

        const aggregate &getAggregate() const {
            return _aggregate;
        }

        color3 getBackground() const {
            return _background;
        }
    };
}  // namespace igi
