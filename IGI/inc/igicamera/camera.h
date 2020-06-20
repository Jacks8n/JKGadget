#pragma once

#include "igientity/transform_base.h"
#include "igigeometry/ray.h"
#include "igimath/const.h"

namespace igi {
    class camera_base : public transformable_base {
        single _near, _far;

      public:
        virtual ray getRay(single x, single y) const = 0;

      protected:
        camera_base(single near, single far) : _near(near), _far(far) { }

        single getNear() const { return _near; }

        single getFar() const { return _far; }

        single getDepth() const { return _far - _near; }
    };

    class camera_orthographic : public camera_base {
        single _width, _height;

        mat4x4f _v2w;

      public:
        camera_orthographic(single width, single height, single near = .1_sg, single far = 1000_sg)
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
        camera_perspective(single left, single right, single bottom, single top,
                           single near = .1_sg, single far = 1000_sg)
            : camera_base(near, far), _left(left), _right(right), _bottom(bottom), _top(top) {
            calculateV2L();
        }

        camera_perspective(single fov, single ratio, single near = .1_sg, single far = 1000_sg)
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
