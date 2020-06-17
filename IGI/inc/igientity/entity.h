#pragma once

#include <memory>
#include "igientity/ITransformable.h"
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
        META_BE(entity, rflite::func_a([](const serializer_t &) { return *(const entity *)nullptr; }))

        entity(const ISurface *surf = nullptr, const IMaterial *mat = nullptr)
            : _surf(surf), _mat(mat) { }

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
