#pragma once

#include "igientity/transform_base.h"
#include "igigeometry/ISurface.h"
#include "igigeometry/bound.h"
#include "igigeometry/ray.h"
#include "igimaterial/IMaterial.h"
#include "igimaterial/interaction.h"

namespace igi {
    class entity : public transformable_base {
        const ISurface *_surf = nullptr;
        const IMaterial *_mat = nullptr;

      public:
        META_BE(entity, rflite::func_a([](const serializer_t &ser, const std::pmr::vector<ISurface *> &surfs, const std::pmr::vector<IMaterial *> &mats) {
                    transform trans      = serialization::Deserialize<transform>(ser);
                    const ISurface *surf = surfs[serialization::Deserialize<int>(ser["surface"])];
                    const IMaterial *mat = mats[serialization::Deserialize<int>(ser["material"])];
                    return rflite::meta_helper::any_ins<entity>(trans, surf, mat);
                }))

        entity(const ISurface *surf = nullptr, const IMaterial *mat = nullptr)
            : transformable_base(), _surf(surf), _mat(mat) { }
        entity(const transform &trans, const ISurface *surf = nullptr, const IMaterial *mat = nullptr)
            : transformable_base(trans), _surf(surf), _mat(mat) { }

        const ISurface &getSurface() const {
            return *_surf;
        }

        const IMaterial &getMaterial() const {
            return *_mat;
        }

        void setSurface(const ISurface *surf) {
            _surf = surf;
        }

        void setMaterial(const IMaterial *mat) {
            _mat = mat;
        }

        bool isHit(const ray &r) const {
            return _surf->isHit(r, getTransform());
        }

        bool tryHit(ray &r, interaction *res) const {
            if (!_surf->tryHit(r, getTransform(), &res->surface))
                return false;

            res->material = _mat;
            return true;
        }

        bound_t getBound() const {
            return _surf->getBound(getTransform());
        }
    };
}  // namespace igi
