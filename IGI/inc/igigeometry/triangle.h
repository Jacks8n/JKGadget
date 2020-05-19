#include <functional>
#include <iterator>
#include <memory_resource>
#include "igigeometry/ISurface.h"

namespace igi {
    using vertex_index_t = uint32_t;

    class triangle_mesh;

    // 0. `triangle`s must be constructed by `triangle_mesh`
    // 1. vertices are in anti-clockwise order
    class triangle : public ISurface {
        friend class triangle_mesh;

        constexpr triangle(vertex_index_t i0, vertex_index_t i1, vertex_index_t i2,
                           const triangle_mesh &mesh)
            : _indices { i0, i1, i2 }, _mesh(mesh) { }

      public:
        single getArea() const override;

        bound_t getBound(const transform &trans) const override;

        bool isHit(const ray &r, const transform &) const override;
        bool tryHit(ray &r, const transform &, surface_interaction *res) const override;

      private:
        const vertex_index_t _indices[3];
        const triangle_mesh &_mesh;

        vec3f getPos(size_t index) const;
        std::tuple<vec3f, vec3f, vec3f> getPos() const;
        vec2f getUV(size_t index) const;
        std::tuple<vec3f, vec3f> getEdge(size_t index) const;
        std::tuple<vec2f, vec2f, vec2f> getUV() const;
    };

    class triangle_mesh {
        friend class triangle;

        template <typename T>
        class triangle_getter : std::iterator_traits<triangle *> {
            T _it;
            const triangle_mesh &_mesh;

          public:
            triangle_getter(const triangle_mesh &mesh, const T &it) : _it(it), _mesh(mesh) { }
            triangle_getter(const triangle_mesh &mesh, T &&it) : _it(it), _mesh(mesh) { }

            triangle operator*() const {
                return triangle(_it[0], _it[1], _it[2], _mesh);
            }

            triangle_getter &operator++() {
                ++++++_it;
                return *this;
            }

            bool operator!=(const triangle_getter &o) const {
                return _it != o._it;
            }
        };

      public:
        using allocator_type = typename std::pmr::vector<void *>::allocator_type;

        template <typename TPosItLo, typename TPosItHi, typename TIndexItLo, typename TIndexItHi>
        triangle_mesh(TPosItLo &&posLo, TPosItHi &&posHi, TIndexItLo &&indexLo, TIndexItHi &&indexHi,
                      const allocator_type &alloc)
            : _positions(std::forward<TPosItLo>(posLo), std::forward<TPosItHi>(posHi), alloc),
              _triangles(triangle_getter(std::forward<TIndexItLo>(indexLo), *this),
                         triangle_getter(std::forward<TIndexItHi>(indexHi), *this), alloc) { }

        auto cbegin() const {
            return _triangles.cbegin();
        }

        auto cend() const {
            return _triangles.cend();
        }

      private:
        const std::pmr::vector<vec3f> _positions;
        const std::pmr::vector<vec2f> _uvs;
        const std::pmr::vector<triangle> _triangles;

        vec3f getPos(size_t index) const {
            return _positions[index];
        }

        vec2f getUV(size_t index) const {
            if (_uvs.size())
                return _uvs[index];

            switch (index) {
                case 0: return vec2f(1_sg, 1_sg);
                case 1: return vec2f(1_sg, 0_sg);
                default: return vec2f(1_sg, 1_sg);
            }
        }
    };

    inline vec3f triangle::getPos(size_t index) const {
        return _mesh.getPos(_indices[index]);
    }

    inline std::tuple<vec3f, vec3f, vec3f> triangle::getPos() const {
        return std::make_tuple(getPos(0), getPos(1), getPos(2));
    }

    inline vec2f triangle::getUV(size_t index) const {
        return _mesh.getUV(_indices[index]);
    }

    inline std::tuple<vec2f, vec2f, vec2f> triangle::getUV() const {
        return std::forward_as_tuple(getUV(0), getUV(1), getUV(2));
    }

    inline std::tuple<vec3f, vec3f> triangle::getEdge(size_t index) const {
        return std::forward_as_tuple(getPos(index), getPos((index + 1) % 3));
    }

    inline single triangle::getArea() const {
        auto [a, b, c] = getPos();
        return Cross(b - a, c - a).magnitude();
    }

    inline bound_t triangle::getBound(const transform &trans) const {
        auto [a, b, c] = getPos();

        vec3f ta = trans.mulPos(getPos(0));
        return bound_t(ta, ta).extend(trans.mulPos(b)).extend(trans.mulPos(c));
    }

    inline bool triangle::isHit(const ray &r, const transform &trans) const {
        ray tmp = r;
        surface_interaction si;
        return tryHit(tmp, trans, &si);
    }
}  // namespace igi
