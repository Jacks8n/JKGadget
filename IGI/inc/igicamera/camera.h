#pragma once

#include "igientity/transform_base.h"
#include "igigeometry/ray.h"
#include "igimath/const.h"

namespace igi {
    class camera_base : public transformable_base {
        single _near, _far;

        mat4x4f _v2w;

      public:
        static constexpr single DefaultNear = SingleEpsilon;
        static constexpr single DefaultFar  = SingleLarge;

        template <typename T>
        class configuration {
            friend class camera_base;

            single _near = DefaultNear;
            single _far  = DefaultFar;

          public:
            constexpr configuration(single near = DefaultNear, single far = DefaultFar)
                : _near(near), _far(far) { }

            constexpr T &setNear(single near) {
                return _near = near, *static_cast<T *>(this);
            }

            constexpr T &setFar(single far) {
                return _far = far, *static_cast<T *>(this);
            }
        };

        META_BE_RT(camera_base)

        ray getRay(vec2f uv) const {
            ray r;
            r.reset(_v2w.mulPos(vec3f(uv, 0_sg)), _v2w.mulPos(vec3f(uv, 1_sg)));
            r.normalizeDirection();
            return r;
        }

        single getNear() const { return _near; }

        single getFar() const { return _far; }

        single getDepth() const { return getFar() - getNear(); }

      protected:
        template <typename T>
        camera_base(const configuration<T> &config) : _near(config._near), _far(config._far) { }

        void setProjection(const mat4x4f &proj) {
            _v2w = getTransform() * proj;
        }
    };

    class camera_orthographic : public camera_base {
      public:
        static constexpr single DefaultWidth  = 10_sg;
        static constexpr single DefaultHeight = 10_sg;

        class configuration : public camera_base::configuration<configuration> {
            friend class camera_orthographic;

            single _width;
            single _height;

          public:
            constexpr configuration(single width = DefaultWidth, single height = DefaultHeight)
                : camera_base::configuration<configuration>(), _width(width), _height(height) { }

            constexpr configuration &setWidth(single width) {
                return _width = width, *this;
            }

            constexpr configuration &setHeight(single height) {
                return _height = height, *this;
            }
        };

        META_BE_RT(camera_orthographic, deser_pmr_func_a<camera_base>([](const serializer_t &ser) {
                       single w = serialization::Deserialize<single>(ser["width"]);
                       single h = serialization::Deserialize<single>(ser["height"]);

                       IGI_SERIALIZE_OPTIONAL(single, near, DefaultNear, ser);
                       IGI_SERIALIZE_OPTIONAL(single, far, DefaultFar, ser);

                       camera_base *camera = context::New<camera_orthographic>(configuration(w, h).setNear(near).setFar(far));
                       return camera;
                   }),
                   ser_pmr_name_a("orthographic"))

        camera_orthographic(const configuration &config) : camera_base(config) {
            setProjection(mat4x4f(config._width, 0_sg, 0_sg, -config._width * .5_sg,
                                  0_sg, config._height, 0_sg, -config._height * .5_sg,
                                  0_sg, 0_sg, getDepth(), getNear(),
                                  0_sg, 0_sg, 0_sg, 1_sg));
        }
    };

    class camera_perspective : public camera_base {
        static constexpr single DefaultFOV   = 70_sg;
        static constexpr single DefaultRatio = 1_sg;

        single _left, _right, _bottom, _top;

      public:
        struct configuration : public camera_base::configuration<configuration> {
            single _left, _right, _bottom, _top;

          public:
            constexpr configuration(single fov = DefaultFOV, single ratio = DefaultRatio) {
                _left   = -(_right = tan(fov * Deg2Rad * .5_sg));
                _bottom = -(_top = _right * ratio);
            }
        };

        META_BE_RT(camera_perspective, deser_pmr_func_a<camera_base>([](const serializer_t &ser) {
                       IGI_SERIALIZE_OPTIONAL(single, fov, DefaultFOV, ser);
                       IGI_SERIALIZE_OPTIONAL(single, ratio, DefaultRatio, ser);
                       IGI_SERIALIZE_OPTIONAL(single, near, DefaultNear, ser);
                       IGI_SERIALIZE_OPTIONAL(single, far, DefaultFar, ser);

                       camera_base *camera = context::New<camera_perspective>(configuration(fov, ratio).setNear(near).setFar(far));
                       return camera;
                   }),
                   ser_pmr_name_a("perspective"))

        camera_perspective(const configuration &config) : camera_base(config) {
            setProjection(mat4x4f(config._right - config._left, 0_sg, 0_sg, config._left,
                                  0_sg, config._top - config._bottom, 0_sg, config._bottom,
                                  0_sg, 0_sg, 0_sg, 1_sg,
                                  0_sg, 0_sg, getDepth(), getNear()));
        }
    };
}  // namespace igi
