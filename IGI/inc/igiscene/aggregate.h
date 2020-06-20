#pragma once

#include <functional>
#include <memory_resource>
#include <stack>
#include "igientity/entity.h"

namespace igi {
    class aggregate {
        struct node {
            bool childIsLeaf[2];
            size_t children[2];
            bound_t bound;

            node() : childIsLeaf { false, false }, children { 0, 0 } { }
            node(bool leftIsLeaf, size_t left, bool rightIsLeaf, size_t right, bound_t bound)
                : childIsLeaf { leftIsLeaf, rightIsLeaf }, children { left, right }, bound(bound) { }
        };

        struct leaf {
            const entity *entity;
            bound_t bound;

            leaf() : entity(nullptr) { }
            constexpr leaf(const igi::entity *ep, bound_t b) : entity(ep), bound(b) { }
        };

        template <typename TIt>
        class leaf_getter : public std::iterator_traits<TIt> {
            TIt _it;

          public:
            explicit constexpr leaf_getter(const TIt &it) : _it(it) { }

            constexpr leaf operator*() const {
                const entity &e = *_it;
                return leaf(&e, e.getBound());
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

      public:
        using initializer_list_t = std::initializer_list<const std::reference_wrapper<entity>>;
        using allocator_type     = std::pmr::polymorphic_allocator<leaf>;
        using itr_stack_t        = std::stack<const void *, std::pmr::vector<const void *>>;

        aggregate(const allocator_type &alloc)
            : _nodes(alloc), _leaves(alloc) { }
        aggregate(const aggregate &o, const allocator_type &alloc)
            : _nodes(o._nodes, alloc), _leaves(o._leaves, alloc) { }
        aggregate(aggregate &&o, const allocator_type &alloc)
            : _nodes(std::move(o._nodes), alloc), _leaves(o._leaves, alloc) { }

        template <typename TIt>
        aggregate(TIt &&entities, size_t n, const allocator_type &alloc, const allocator_type &tempAlloc)
            : _nodes(alloc), _leaves(n, alloc) {
            initBuild(std::forward<TIt>(entities), n, tempAlloc);
        }
        template <typename TIt>
        aggregate(TIt &&entities, size_t n, const allocator_type &alloc) : aggregate(std::forward<TIt>(entities), n, alloc, alloc) { }

        aggregate &operator=(const aggregate &) = delete;
        aggregate &operator=(aggregate &&) = delete;

        ~aggregate() = default;

        bool isHit(const ray &r, itr_stack_t &itrtmp) const {
            return hit_impl<true>(r, itrtmp,
                                  [](const entity &e, const ray &r) { return e.isHit(r); });
        }

        bool tryHit(ray &r, interaction *res, itr_stack_t &itrtmp) const {
            return hit_impl<false>(r, itrtmp,
                                   [=](const entity &e, ray &r) { return e.tryHit(r, res); });
        }

      private:
        std::pmr::vector<node> _nodes;
        std::pmr::vector<leaf> _leaves;

        void initBuild(allocator_type tempAlloc);

        template <typename TIt>
        void initBuild(TIt &&entities, size_t n, allocator_type alloc) {
            if (!n) return;
            std::uninitialized_move_n(leaf_getter(entities), n, _leaves.begin());

            if (n < 3) {
                if (n == 1)
                    _nodes.emplace_back(true, 0, false, 0, _leaves[0].bound);
                else {
                    bound_t b = _leaves[0].bound;
                    _nodes.emplace_back(true, 0, true, 1, b.extend(_leaves[1].bound));
                }
                return;
            }

            initBuild(alloc);
        }

        template <bool FindFirst, typename TRay, typename TFn>
        bool hit_impl(TRay &&r, itr_stack_t &itrtmp, TFn &&fn) const {
            const size_t emptySize = itrtmp.size();

            bool hit = false;
            itrtmp.push(_nodes.data());
            do {
                const node *curr = static_cast<const node *>(itrtmp.top());
                if (curr->bound.isHit(r)) {
                    for (size_t i = 0; i < 2; i++)
                        if (curr->childIsLeaf[i]) {
                            const leaf &l = _leaves[curr->children[i]];
                            if (l.bound.isHit(r) && fn(*l.entity, r)) {
                                hit = true;

                                if constexpr (FindFirst) {
                                    do
                                        itrtmp.pop();
                                    while (itrtmp.size() > emptySize);
                                    goto end;
                                }
                            }
                        }
                        else if (curr->children[i])
                            itrtmp.push(&_nodes[curr->children[i]]);
                }

                itrtmp.pop();
            } while (itrtmp.size() > emptySize);

end:
            return hit;
        }
    };
}  // namespace igi
