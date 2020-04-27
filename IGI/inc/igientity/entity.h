#pragma once

#include <memory>
#include "igimath/ray.h"
#include "igigeometry/ISurface.h"
#include "igimaterial/interaction.h"
#include "igimaterial/IMaterial.h"
#include "igientity/ITransformable.h"

namespace igi {
    class entity : public transformable_base {
        const ISurface* _surf;
        const IMaterial* _mat;

    public:
        entity() = default;
        entity(const entity&) = default;
        entity(entity&&) = default;
        entity(const ISurface& surf, const IMaterial& mat) : _surf(&surf), _mat(&mat) {}

        entity& operator=(const entity&) = default;
        entity& operator=(entity&&) = default;

        ~entity() = default;

        void setSurface(const ISurface& surf) {
            _surf = &surf;
        }

        void setMaterial(const IMaterial& mat) {
            _mat = &mat;
        }

        bool isHit(const ray& r) const {
            return _surf->isHit(toLocalRay(r));
        }

        bool tryHit(ray& r, interaction& res) const {
            ray localr = toLocalRay(r);
            surface_hit hit;

            if (!_surf->tryHit(localr, hit))
                return false;

            r.setT(localr.getT());
            res.material = _mat;
            res.normal = getTransform().mulNormal(hit.normal);
            res.position = getTransform().mulPos(hit.position);
            return true;
        }

    private:
        ray toLocalRay(const ray& r) const {
            ray lr;
            lr.setOrigin(getTransform().mulPosInv(r.getOrigin()));
            lr.setEndpoint(getTransform().mulPosInv(r.getEndpoint()));
            lr.normalizeDirection();
            return lr;
        }
    };
}
