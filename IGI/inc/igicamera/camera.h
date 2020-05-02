#pragma once

#include "igientity/ITransformable.h"
#include "igiintegrator/IIntegrator.h"
#include "igimath/const.h"
#include "igitexture/texture.h"

namespace igi {
    class camera_base : public transformable_base {
        unit_square_distribution _usd;

      public:
        template <typename TInt>
        void render(texture_rgb &res, TInt &&integrator, size_t spp = 1) const {
            pcg32 rand;

            ray r;
            vec2f xy, samp;
            single w = res.getWidth(), h = res.getHeight();
            color_rgb pixel;
            for (size_t i = 0; i < res.getWidth(); i++) {
                xy[0] = AsSingle(i);
                for (size_t j = 0; j < res.getHeight(); j++) {
                    xy[1] = AsSingle(j);

                    pixel = palette_rgb::black;
                    for (size_t k = 0; k < spp; k++) {
                        samp  = _usd(rand);
                        r     = getRay((xy[0] + samp[0]) / w, (xy[1] + samp[1]) / h);
                        pixel = pixel + integrator.integrate(r);
                    }
                    res.get(i, j) = pixel;
                }
            }
        }

      protected:
        single _near, _far;

        camera_base(single near, single far) : _near(near), _far(far) { }

        virtual ray getRay(single x, single y) const = 0;
    };

    class camera_orthographic : public camera_base {
        single _width, _height;

        mat4x4f _v2w;

      public:
        camera_orthographic(single width, single height, single near = .1, single far = 1000)
            : camera_base(near, far), _width(width), _height(height) {
            calculateV2W();
        }

      protected:
        ray getRay(single x, single y) const override {
            ray r(_v2w.mulPos(vec3f(x, y, 0)), _v2w.mulVec(vec3f(0, 0, 1)), 1);
            r.normalizeDirection();
            return r;
        }

      private:
        void calculateV2W() {
            _v2w = getTransform() * mat4x4f(_width, 0, 0, -_width * AsSingle(.5), 0, _height, 0, -_height * AsSingle(.5), 0, 0, _far - _near, _near, 0, 0, 0, 1);
        }
    };

    class camera_perspective : public camera_base {
        single _left, _right, _bottom, _top;

        mat4x4f _v2l;

      public:
        camera_perspective(single left, single right, single bottom, single top, single near = .1, single far = 1000)
            : camera_base(near, far), _left(left), _right(right), _bottom(bottom), _top(top) {
            calculateV2L();
        }

        camera_perspective(single fov, single ratio, single near = .1, single far = 1000)
            : camera_base(near, far) {
            _left   = -(_right = tan(fov * Deg2Rad * AsSingle(.5)));
            _bottom = -(_top = _right * ratio);
            calculateV2L();
        }

      protected:
        ray getRay(single x, single y) const override {
            const transform &trans = getTransform();
            ray r;
            r.setOrigin(trans.mulPos(_v2l.mulPos(vec3f(x, y, 0))));
            r.setEndpoint(trans.mulPos(_v2l.mulPos(vec3f(x, y, 1))));
            r.normalizeDirection();
            return r;
        }

      private:
        void calculateV2L() {
            _v2l = mat4x4f(_right - _left, 0, 0, _left,
                           0, _top - _bottom, 0, _bottom,
                           0, 0, 0, 1,
                           0, 0, _far - _near, _near);
        }
    };
}  // namespace igi
