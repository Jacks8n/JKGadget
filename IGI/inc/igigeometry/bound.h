#pragma once

#include "igigeometry/ray.h"
#include "igimath/vec.h"

namespace igi {
    class aabb {
        vec3f _min, _max;

      public:
        aabb() = default;
        constexpr aabb(const vec3f &half) : _min(-half), _max(half) { }
        constexpr aabb(const vec3f &min, const vec3f &max) : _min(min), _max(max) { }

        static constexpr aabb Inf() {
            constexpr aabb inf(-vec3f::Infinity(), vec3f::Infinity());
            return inf;
        }

        static constexpr aabb NegInf() {
            constexpr aabb inf(vec3f::Infinity(), -vec3f::Infinity());
            return inf;
        }

        constexpr const vec3f &getMin() const {
            return _min;
        }

        constexpr single getMin(size_t dim) const {
            return _min[dim];
        }

        constexpr const vec3f &getMax() const {
            return _max;
        }

        constexpr single getMax(size_t dim) const {
            return _max[dim];
        }

        constexpr vec3f getDiagonal() const {
            return _max - _min;
        }

        constexpr single getSize(size_t dim) const {
            return _max[dim] - _min[dim];
        }

        constexpr vec2f getInterval(size_t dim) const {
            return vec2f(getMin(dim), getMax(dim));
        }

        aabb &extend(const vec3f &p) {
            for (size_t i = 0; i < 3; i++)
                if (Lesscf(p[i], _min[i]))
                    _min[i] = p[i];
                else if (Lesscf(_max[i], p[i]))
                    _max[i] = p[i];
            return *this;
        }

        aabb &extend(const aabb &b) {
            return extend(b._min).extend(b._max);
        }

        aabb transform(const igi::transform &trans) const {
            aabb res = NegInf();
            for (size_t i = 0; i < 7; i++)
                res.extend(trans.mulPos(operator[](i)));
            return res;
        }

        bool isHit(const ray &r) const {
            vec3f e = r.getEndpoint();
            vec3f o = r.getOrigin();

            for (size_t i = 0; i < 3; i++) {
                auto [min, max] = Ascendcf(e[i], o[i]);
                if (!Overlapcf(min, max, _min[i], _max[i]))
                    return false;
            }
            return true;
        }

        constexpr vec3f operator[](size_t index) const {
            return vec3f(index & 1 ? _max[0] : _min[0],
                         index & 2 ? _max[1] : _min[1],
                         index & 4 ? _max[2] : _min[2]);
        }
    };

    using bound_t = aabb;
}  // namespace igi