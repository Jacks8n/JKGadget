#pragma once

#include "igiscene/aggregate.h"
#include "igiutilities/shared_vector.h"

namespace igi {
    class scene {
        std::shared_ptr<IMaterial *[]> _materials;

        std::shared_ptr<ISurface *[]> _surfaces;

        aggregate _aggregate;

        color3 _background;

      public:
        META_BE(scene, rflite::func_a([](const serializer_t &ser) {
                    const serializer_t &mser = ser["material"];
                    const serializer_t &sser = ser["surface"];
                    const serializer_t &eser = ser["entity"];

                    IGI_SERIALIZE_OPTIONAL(color3, background, palette::black, ser);

                    constexpr auto policy = [](const serializer_t &ser) { return ser["type"].GetString(); };

                    shared_vector<IMaterial *> mats = serialization::DeserializePmrArray<IMaterial, shared_vector>(mser, policy);
                    shared_vector<ISurface *> surfs = serialization::DeserializePmrArray<ISurface, shared_vector>(sser, policy);
                    shared_vector<entity> ents      = serialization::DeserializeArray<entity, shared_vector>(eser, mats, surfs);

                    return rflite::meta_helper::any_new<scene>(
                        std::move(mats).as_shared_ptr(),
                        std::move(surfs).as_shared_ptr(),
                        std::move(ents).as_shared_ptr(), ents.size(), background);
                }))

        scene(std::shared_ptr<IMaterial *[]> mats, std::shared_ptr<ISurface *[]> surfs,
              std::shared_ptr<entity[]> entities, size_t nentities, const color3 &background)
            : _materials(std::move(mats)), _surfaces(std::move(surfs)),
              _aggregate(entities.get(), nentities), _background(background) { }

        const aggregate &getAggregate() const {
            return _aggregate;
        }

        color3 getBackground() const {
            return _background;
        }
    };
}  // namespace igi
