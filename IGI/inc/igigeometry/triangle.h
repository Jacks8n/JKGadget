#include <iterator>
#include <memory_resource>
#include "igigeometry/ISurface.h"
#include "igiutilities/igiassert.h"

namespace igi {
    class triangle_mesh;

    /// <summary>
    /// <para> 1. triangles must be constructed by triangle_mesh </para>
    /// <para> 2. vertices are in anti-clockwise order </para>
    /// </summary>
    class triangle : public ISurface {
        friend class triangle_mesh;

      public:
        using index_t = uint32_t;

        constexpr triangle() : _indices { 0 }, _mesh(nullptr) { }

        constexpr triangle(index_t i0, index_t i1, index_t i2,
                           const triangle_mesh *mesh)
            : _indices { i0, i1, i2 }, _mesh(mesh) { }

        single getArea() const override;

        bound_t getBound(const transform &trans) const override;

        bool isHit(const ray &r, const transform &) const override;
        bool tryHit(ray &r, const transform &, surface_interaction *res) const override;

        decltype(auto) getPos(size_t index) const;
        decltype(auto) getUV(size_t index) const;

        decltype(auto) getPos() const;
        decltype(auto) getEdge(size_t index) const;
        decltype(auto) getUV() const;

      private:
        index_t _indices[3];
        const triangle_mesh *_mesh;
    };

    enum class triangle_topology {
        list,
        fan
    };

    class triangle_mesh {
        friend class triangle;

      public:
        triangle_mesh() : _positionInit(false), _uvInit(false), _triangleInit(false) { }
        triangle_mesh(triangle_mesh &&) = default;

        ~triangle_mesh() {
            if (_positionInit)
                _positions.~vector();
            if (_uvInit)
                _uvs.~vector();
            if (_triangleInit)
                _triangles.~vector();
        }

        template <typename TLo, typename THi>
        void setPos(TLo &&posLo, THi &&posHi) {
            SetVector(_positions, _positionInit, std::forward<TLo>(posLo), std::forward<THi>(posHi), context::GetTypedAllocator<vec3f>());
        }

        template <typename TLo, typename THi>
        void setUV(TLo &&uvLo, THi &&uvHi) {
            SetVector(_uvs, _uvInit, std::forward<TLo>(uvLo), std::forward<THi>(uvHi), context::GetTypedAllocator<vec2f>());
        }

        template <typename TLo, typename THi>
        void setTriangle(TLo &&triangleLo, THi &&triangleHi) {
            SetVector(_triangles, _triangleInit, std::forward<TLo>(triangleLo), std::forward<THi>(triangleHi), context::GetTypedAllocator<triangle>());
        }

        void setTriangle(triangle_topology topology) {
            assert(_positionInit);
            igiassert(_positions.size() <= std::numeric_limits<triangle::index_t>::max(),
                      "vertex count exceeds allowed maximum index");

            size_t ntriangle = 0;
            switch (topology) {
                case igi::triangle_topology::list:
                    assert(_positions.size() >= 3 && !(_positions.size() % 3));

                    ntriangle = _positions.size() / 3;
                    break;
                case igi::triangle_topology::fan:
                    assert(_positions.size() >= 3);

                    ntriangle = _positions.size() - 2;
                    break;
            }
            if (!ntriangle)
                return;

            InitVector(_triangles, _triangleInit, ntriangle, context::GetTypedAllocator<triangle>());

            switch (topology) {
                case igi::triangle_topology::list:
                    for (triangle::index_t i = 0; i < _positions.size(); i += 3)
                        _triangles.emplace_back(i, i + 1, i + 2, this);
                    break;
                case igi::triangle_topology::fan:
                    for (triangle::index_t i = 1; i < _positions.size(); i++)
                        _triangles.emplace_back(0, i, i + 1, this);
                    break;
            }
        }

        decltype(auto) cbegin() const {
            return _triangles.cbegin();
        }

        decltype(auto) cend() const {
            return _triangles.cend();
        }

        decltype(auto) begin() const {
            return _triangles.begin();
        }

        decltype(auto) end() const {
            return _triangles.end();
        }

        decltype(auto) begin() {
            return _triangles.begin();
        }

        decltype(auto) end() {
            return _triangles.end();
        }

        template <typename T>
        void addTrianglesTo(T &&it) {
            for (auto &i : *this) {
                if constexpr (std::is_pointer_v<T>)
                    *it = &i;
                else
                    *it = i;
                ++it;
            }
        }

        const vec3f &getPos(size_t index) const {
            assert(_positionInit);
            assert(index < _positions.size());

            return _positions[index];
        }

        const vec2f &getUV(size_t index) const {
            assert(_uvInit);
            assert(index < _uvs.size());

            return _uvs[index];
        }

      private:
        std::pmr::vector<vec3f> _positions;
        std::pmr::vector<vec2f> _uvs;
        std::pmr::vector<triangle> _triangles;

        bool _positionInit;
        bool _uvInit;
        bool _triangleInit;

        template <typename T, typename TAlloc>
        static void InitVector(std::pmr::vector<T> &vec, bool &flag, size_t size, TAlloc &&alloc) {
            if (flag) {
                if (vec.get_allocator() == alloc) {
                    vec.clear();
                    vec.reserve(size);
                    return;
                }

                vec.~vector();
            }

            new (&vec) std::pmr::vector<T>(size, alloc);
            vec.clear();
            flag = true;
        }

        template <typename T, typename TLo, typename THi, typename TAlloc>
        static void SetVector(std::pmr::vector<T> &vec, bool &flag, TLo &&lo, THi &&hi, TAlloc &&alloc) {
            size_t size = hi - lo;

            InitVector(vec, flag, size, alloc);

            vec.insert(vec.end(), std::forward<TLo>(lo), std::forward<THi>(hi));
        }
    };

    inline decltype(auto) triangle::getPos(size_t index) const {
        return _mesh->getPos(_indices[index]);
    }

    inline decltype(auto) triangle::getUV(size_t index) const {
        static constexpr vec2f DefaultUVs[] {
            vec2f(0_sg, 0_sg),
            vec2f(0_sg, 1_sg),
            vec2f(1_sg, 0_sg)
        };

        if (_mesh->_uvInit)
            return _mesh->getUV(_indices[index]);

        assert(index < std::size(DefaultUVs));
        return DefaultUVs[index];
    }

    inline decltype(auto) triangle::getPos() const {
        return std::make_tuple(getPos(0), getPos(1), getPos(2));
    }

    inline decltype(auto) triangle::getUV() const {
        return std::forward_as_tuple(getUV(0), getUV(1), getUV(2));
    }

    inline decltype(auto) triangle::getEdge(size_t index) const {
        return std::forward_as_tuple(getPos(index), getPos((index + 1) % 3));
    }

    inline single triangle::getArea() const {
        const auto &[a, b, c] = getPos();
        return Cross(b - a, c - a).magnitude();
    }

    inline bound_t triangle::getBound(const transform &trans) const {
        const auto &[a, b, c] = getPos();

        vec3f ta = trans.mulPos(getPos(0));
        return bound_t(ta, ta).extend(trans.mulPos(b)).extend(trans.mulPos(c));
    }
}  // namespace igi
