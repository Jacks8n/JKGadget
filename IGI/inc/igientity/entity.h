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

        void setSurface(const ISurface *surf) {
            _surf = surf;
        }

        void setMaterial(const IMaterial *mat) {
            _mat = mat;
        }

        bool isHit(const ray &r) const {
            return _surf->isHit(toLocalRay(r));
        }

        bool tryHit(ray &r, interaction *res) const {
            ray localr = toLocalRay(r);

            if (!_surf->tryHit(localr, &res->surface))
                return false;

            r.setT(localr.getT());
            res->material = _mat;

            const transform &trans = getTransform();
            res->surface.position  = trans.mulPos(res->surface.position);
            res->surface.normal    = trans.mulNormal(res->surface.normal);
            res->surface.dpdu      = trans.mulVec(res->surface.dpdu);
            res->surface.dpdv      = trans.mulVec(res->surface.dpdv);

            return true;
        }

        bound_t getBound() const {
            return _surf->getBound().transform(getTransform());
        }

      private:
        ray toLocalRay(const ray &r) const {
            ray lr;

            const transform &trans = getTransform();
            lr.setOrigin(trans.mulPosInv(r.getOrigin()));
            lr.setEndpoint(trans.mulPosInv(r.getEndpoint()));
            lr.normalizeDirection();

            return lr;
        }
    };
}  // namespace igi
