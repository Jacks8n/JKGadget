#pragma once

#include "igientity/transform_base.h"
#include "igigeometry/ray.h"
#include "igimath/const.h"

namespace igi {
    class camera_base : public transformable_base {
        single _near, _far;

      public:
        static constexpr single DefaultNear = .1_sg;
        static constexpr single DefaultFar  = 1000_sg;

        META_BE_RT(camera_base)

        virtual ray getRay(single x, single y) const = 0;

        single getNear() const { return _near; }

        void setNear(single n) { _near = n; }

        single getFar() const { return _far; }

        void setFar(single f) { _far = f; }

        single getDepth() const { return _far - _near; }

      protected:
        camera_base(single near, single far) : _near(near), _far(far) { }
    };

    class camera_orthographic : public camera_base {
        single _width, _height;

        mat4x4f _v2w;

      public:
        META_BE_RT(camera_orthographic, deser_pmr_func_a<camera_base>([](const serializer_t &ser) {
                       single w = serialization::Deserialize<single>(ser["width"]);
                       single h = serialization::Deserialize<single>(ser["height"]);

                       IGI_SERIALIZE_OPTIONAL(single, near, DefaultNear, ser);
                       IGI_SERIALIZE_OPTIONAL(single, far, DefaultFar, ser);

                       camera_base *camera = context::New<camera_orthographic>(w, h, near, far);
                       return camera;
                   }),
                   ser_pmr_name_a("orthographic"))

        camera_orthographic(single width, single height, single near = DefaultNear, single far = DefaultFar)
            : camera_base(near, far), _width(width), _height(height) {
            calculateV2W();
        }

        ray getRay(single x, single y) const override {
            ray r(_v2w.mulPos(vec3f(x, y, 0_sg)), _v2w.mulVec(vec3f(0_sg, 0_sg, 1_sg)), 1_sg);
            r.normalizeDirection();
            return r;
        }

      private:
        void calculateV2W() {
            _v2w = getTransform()
                   * mat4x4f(_width, 0_sg, 0_sg, -_width * .5_sg,
                             0_sg, _height, 0_sg, -_height * .5_sg,
                             0_sg, 0_sg, getDepth(), getNear(),
                             0_sg, 0_sg, 0_sg, 1_sg);
        }
    };

    class camera_perspective : public camera_base {
        single _left, _right, _bottom, _top;

        mat4x4f _v2l;

      public:
        static constexpr single DefaultFOV   = 70_sg;
        static constexpr single DefaultRatio = 1_sg;

        META_BE_RT(camera_perspective, deser_pmr_func_a<camera_base>([](const serializer_t &ser) {
                       IGI_SERIALIZE_OPTIONAL(single, fov, DefaultFOV, ser);
                       IGI_SERIALIZE_OPTIONAL(single, ratio, DefaultRatio, ser);
                       IGI_SERIALIZE_OPTIONAL(single, near, DefaultNear, ser);
                       IGI_SERIALIZE_OPTIONAL(single, far, DefaultFar, ser);

                       camera_base *camera = context::New<camera_perspective>(fov, ratio, near, far);
                       return camera;
                   }),
                   ser_pmr_name_a("perspective"))

        camera_perspective(single fov = DefaultFOV, single ratio = DefaultRatio, single near = DefaultNear, single far = DefaultFar)
            : camera_base(near, far) {
            _left   = -(_right = tan(fov * Deg2Rad * .5_sg));
            _bottom = -(_top = _right * ratio);
            calculateV2L();
        }

        ray getRay(single x, single y) const override {
            const transform &trans = getTransform();
            ray r;
            r.setOrigin(trans.mulPos(_v2l.mulPos(vec3f(x, y, 0_sg))));
            r.setEndpoint(trans.mulPos(_v2l.mulPos(vec3f(x, y, 1_sg))));
            r.normalizeDirection();
            return r;
        }

      private:
        void calculateV2L() {
            _v2l = mat4x4f(_right - _left, 0_sg, 0_sg, _left,
                           0_sg, _top - _bottom, 0_sg, _bottom,
                           0_sg, 0_sg, 0_sg, 1_sg,
                           0_sg, 0_sg, getDepth(), getNear());
        }
    };
}  // namespace igi
