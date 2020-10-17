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
            constexpr aabb inf(-vec3f::One(SingleInf), vec3f::One(SingleInf));
            return inf;
        }

        static constexpr aabb NegInf() {
            constexpr aabb inf(vec3f::One(SingleInf), vec3f::One(-SingleInf));
            return inf;
        }

        constexpr const vec3f &getMin() const {
            return _min;
        }

        constexpr single getMin(size_t dim) const {
            return _min[dim];
        }

        void setMin(const vec3f &min) {
            _min = min;
        }

        void setMin(size_t dim, single coord) {
            _min[dim] = coord;
        }

        constexpr const vec3f &getMax() const {
            return _max;
        }

        constexpr single getMax(size_t dim) const {
            return _max[dim];
        }

        void setMax(const vec3f &max) {
            _max = max;
        }

        void setMax(size_t dim, single coord) {
            _max[dim] = coord;
        }

        constexpr vec3f getDiagonal() const {
            return _max - _min;
        }

        constexpr single getSize(size_t dim) const {
            return _max[dim] - _min[dim];
        }

        constexpr auto getInterval(size_t dim) const {
            return std::make_pair(getMin(dim), getMax(dim));
        }

        aabb &extend(const vec3f &p) {
            for (size_t i = 0; i < 3; i++)
                if (p[i] < _min[i])
                    _min[i] = p[i];
                else if (_max[i] < p[i])
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

            single min, max;
            for (size_t i = 0; i < 3; i++) {
                min = e[i] < o[i] ? e[i] : o[i];
                max = e[i] < o[i] ? o[i] : e[i];
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
