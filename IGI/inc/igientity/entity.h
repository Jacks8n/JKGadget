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
        entity()               = default;
        entity(const entity &) = default;
        entity(entity &&)      = default;
        entity(const ISurface *surf, const IMaterial *mat)
            : _surf(surf), _mat(mat) { }

        entity &operator=(const entity &) = default;
        entity &operator=(entity &&) = default;

        ~entity() = default;

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
