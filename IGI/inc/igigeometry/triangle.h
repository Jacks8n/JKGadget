#include <functional>
#include <iterator>
#include <memory_resource>
#include "igigeometry/ISurface.h"

namespace igi {
    struct triangle_vertex {
        vec3f position;
        vec2f uv;

        constexpr triangle_vertex(const vec3f &p, const vec2f &uv)
            : position(p), uv(uv) { }
    };

    // 0. `triangle`s must be constructed by `triangle_mesh`
    // 1. vertices are in anti-clockwise order
    class triangle : public ISurface {
        friend class triangle_mesh;

        constexpr triangle(const triangle_vertex *v0, const triangle_vertex *v1,
                           const triangle_vertex *v2)
            : _vertices { v0, v1, v2 }, _normal(Cross(v1->position - v0->position,
                                                      v2->position - v0->position)
                                                    .normalized()) { }

      public:
        constexpr const vec3f &getPos(size_t index) const {
            return operator[](index).position;
        }

        constexpr const vec2f &getUV(size_t index) const {
            return operator[](index).uv;
        }

        constexpr auto asTuple() const {
            return std::forward_as_tuple(operator[](0), operator[](1), operator[](2));
        }

        constexpr auto getEdge(size_t index) const {
            return std::forward_as_tuple(getPos(index), getPos((index + 1) % 3));
        }

        constexpr auto asPosTuple() const {
            return std::forward_as_tuple(getPos(0), getPos(1), getPos(2));
        }

        constexpr auto asUVTuple() const {
            return std::forward_as_tuple(getUV(0), getUV(1), getUV(2));
        }

        vec3f getPoint(single u, single v) const override { }

        vec3f getNormal(single u, single v) const override {
            return _normal;
        }
        vec3f getPartialU(single u, single v) const override { }
        vec3f getPartialV(single u, single v) const override { }

        single getArea() const override {
            auto [a, b, c] = asPosTuple();

            return Cross(b - a, c - a).magnitude();
        }

        bound_t getBound(const transform &trans) const override {
            auto [a, b, c] = asPosTuple();

            vec3f ta = trans.mulPos(a);
            return bound_t(ta, ta).extend(trans.mulPos(b)).extend(trans.mulPos(c));
        }

        bool isHit(const ray &r) const override { }
        bool tryHit(ray &r, surface_interaction *res) const override {
            single t = Dot(getPos(0) - r.getOrigin(), _normal)
                       / Dot(r.getDirection(), _normal);

            if (!InRangecf(r.getTMin(), r.getT(), t))
                return false;

            vec3f e = r.cast(t);
            for (size_t i = 0; i < 3; i++) {
                auto [a, b] = getEdge(i);
                if (IsPositivecf(Dot(_normal, Cross(b - a, e - a))))
                    return false;
            }

            r.setT(t);

            surface_helper::CalculateInteraction(this, r, e, _normal, uv);
            return true;
        }

        constexpr const triangle_vertex &operator[](size_t index) const {
#ifdef _DEBUG
            if (index > 2) throw;
#endif
            return *_vertices[index];
        }

      private:
        const triangle_vertex *_vertices[3];
        const vec3f _normal;
    };

    class triangle_mesh {
      public:
        class triangle_iterator : std::iterator_traits<const triangle_vertex *> {
            friend class triangle_mesh;

            using vert_iterator  = typename std::pmr::vector<triangle_vertex>::const_iterator;
            using index_iterator = typename std::pmr::vector<size_t>::const_iterator;

            vert_iterator _vertIt;
            index_iterator _indexIt;

            triangle_iterator(const vert_iterator &vertIt, const index_iterator &indexIt)
                : _vertIt(vertIt), _indexIt(indexIt) { }

          public:
            constexpr triangle operator*() {
                return triangle(&_vertIt[_indexIt[0]], &_vertIt[_indexIt[1]], &_vertIt[_indexIt[2]]);
            }

            triangle_iterator &operator++() {
                ++++++_indexIt;
                return *this;
            }

            constexpr bool operator!=(const triangle_iterator &o) const {
                return _vertIt != o._vertIt || _indexIt != o._indexIt;
            }
        };

        using allocator_type = typename std::pmr::vector<triangle_vertex>::allocator_type;

        template <typename TVertItLo, typename TVertItHi, typename TIndexItLo, typename TIndexItHi>
        triangle_mesh(TVertItLo &&vertLo, TVertItHi &&vertHi,
                      TIndexItLo &&indexLo, TIndexItHi &&indexHi, const allocator_type &alloc)
            : _vertices(std::forward<TVertItLo>(vertLo), std::forward<TVertItHi>(vertHi), alloc),
              _indices(std::forward<TIndexItLo>(indexLo), std::forward<TIndexItHi>(indexHi), alloc) { }

        triangle_iterator begin() const {
            return triangle_iterator(_vertices.cbegin(), _indices.cbegin());
        }

        triangle_iterator end() const {
            return triangle_iterator(_vertices.cend(), _indices.cend());
        }

      private:
        std::pmr::vector<triangle_vertex> _vertices;
        std::pmr::vector<size_t> _indices;
    };

    template <typename... Ts>
    class triangle_vertex_iterator;

    template <typename TPosIt, typename TUVIt>
    class triangle_vertex_iterator<TPosIt, TUVIt> : std::iterator_traits<triangle_vertex *> {
        TPosIt _vertIt;
        TUVIt _uvIt;

      public:
        template <typename _TPosIt, typename _TUVIt>
        constexpr triangle_vertex_iterator(_TPosIt &&vertIt, _TUVIt &&uvIt)
            : _vertIt(std::forward<_TPosIt>(vertIt)), _uvIt(std::forward<_TUVIt>(uvIt)) { }

        constexpr triangle_vertex operator*() const {
            return triangle_vertex(*_vertIt, *_uvIt);
        }

        triangle_vertex_iterator &operator++() {
            ++_vertIt;
            ++_uvIt;
            return *this;
        }

        constexpr bool operator!=(const triangle_vertex_iterator &o) const {
            return _vertIt != o._vertIt || _uvIt != o._uvIt;
        }
    };

    template <typename TIt>
    class triangle_vertex_iterator<TIt> : std::iterator_traits<triangle_vertex *> {
        using getter_t = std::function<triangle_vertex(typename std::iterator_traits<TIt>::reference)>;

        TIt _it;
        getter_t _getter;

      public:
        template <typename _TIt, typename _TFn>
        constexpr triangle_vertex_iterator(_TIt &&it, _TFn &&fn)
            : _it(std::forward<_TIt>(it)), _getter(std::forward<_TFn>(fn)) { }

        constexpr triangle_vertex operator*() const {
            return _getter(*_it);
        }

        triangle_vertex_iterator &operator++() {
            ++_it;
            return *this;
        }

        constexpr bool operator!=(const triangle_vertex_iterator &o) const {
            return _it != o._it || _getter != o._getter;
        }
    };

    template <typename TPosIt, typename TUVIt,
              std::enable_if_t<std::is_same_v<typename std::iterator_traits<TPosIt>::value_type, vec3f>, int> = 0,
              std::enable_if_t<std::is_same_v<typename std::iterator_traits<TUVIt>::value_type, vec2f>, int>  = 0>
    triangle_vertex_iterator(TPosIt, TUVIt) -> triangle_vertex_iterator<std::remove_cv_t<TPosIt>, std::remove_cv_t<TUVIt>>;

    template <typename TIt, typename Fn,
              std::enable_if_t<std::is_invocable_v<Fn, typename std::iterator_traits<TIt>::value_type>, int> = 0>
    triangle_vertex_iterator(TIt, Fn) -> triangle_vertex_iterator<TIt>;
}  // namespace igi