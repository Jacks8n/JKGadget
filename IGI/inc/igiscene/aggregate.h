#pragma once

#include <functional>
#include <memory_resource>
#include "igientity/entity.h"

namespace igi {
    class aggregate {
        struct node {
            bool childIsLeaf[2];
            size_t children[2];
            bound_t bound;
        };

        struct leaf {
            const entity *entity;
            bound_t bound;
        };

        template <typename TIt>
        class leaf_getter : public std::iterator_traits<TIt> {
            TIt _it;

          public:
            explicit constexpr leaf_getter(const TIt &it) : _it(it) { }

            constexpr leaf operator*() const {
                const entity &e = *_it;
                return leaf { &e, e.getBound() };
            }

            leaf_getter &operator++() {
                ++_it;
                return *this;
            }

            constexpr auto operator-(const leaf_getter &r) const {
                return _it - r._it;
            }

            constexpr bool operator!=(const leaf_getter &r) const {
                return _it != r._it;
            }
        };

        using initializer_list_t = std::initializer_list<const std::reference_wrapper<entity>>;

      public:
        using allocator_type = std::pmr::polymorphic_allocator<leaf>;

        aggregate(const allocator_type &alloc)
            : _nodes(alloc), _leaves(alloc) { }
        aggregate(const aggregate &o, const allocator_type &alloc)
            : _nodes(o._nodes, alloc), _leaves(o._leaves, alloc) { }
        aggregate(aggregate &&o, const allocator_type &alloc)
            : _nodes(std::move(o._nodes), alloc), _leaves(o._leaves, alloc) { }
        aggregate(const initializer_list_t &il, const allocator_type &alloc)
            : aggregate(il, alloc, alloc) { }
        aggregate(const initializer_list_t &il, const allocator_type &alloc,
                  const allocator_type &tempAlloc)
            : _nodes(alloc), _leaves(il.size(), alloc) {
            initBuild(il, tempAlloc);
        }

        aggregate &operator=(const aggregate &) = delete;
        aggregate &operator=(aggregate &&) = delete;

        ~aggregate() = default;

        void add(const entity &e);

        bool isHit(const ray &r) const;

        bool tryHit(ray &r, interaction *res) const;

      private:
        std::pmr::vector<node> _nodes;
        std::pmr::vector<leaf> _leaves;

        void initBuild(const initializer_list_t &il, allocator_type alloc);
    };
}  // namespace igi