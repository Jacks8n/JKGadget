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

            node() { }
            node(bool leftIsLeaf, size_t left, bool rightIsLeaf, size_t right, bound_t bound)
                : childIsLeaf { leftIsLeaf, rightIsLeaf }, children { left, right }, bound(bound) { }
        };

        struct leaf {
            const entity *entity;
            bound_t bound;

            leaf() { }
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

        using initializer_list_t = std::initializer_list<const std::reference_wrapper<entity>>;

      public:
        using allocator_type = std::pmr::polymorphic_allocator<leaf>;
        using itr_stack_t    = std::stack<const void *, std::pmr::vector<const void *>>;

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

        bool isHit(const ray &r, itr_stack_t &itrtmp) const {
            return hit_impl(r, itrtmp,
                            [](const entity &e, const ray &r) { return e.isHit(r); });
        }

        bool tryHit(ray &r, interaction *res, itr_stack_t &itrtmp) const {
            return hit_impl(r, itrtmp,
                            [=](const entity &e, ray &r) { return e.tryHit(r, res); });
        }

      private:
        std::pmr::vector<node> _nodes;
        std::pmr::vector<leaf> _leaves;

        void initBuild(const initializer_list_t &il, allocator_type alloc);

        template <typename TRay, typename TFn>
        bool hit_impl(TRay &&r, itr_stack_t &itrtmp, TFn &&fn) const {
            size_t emptySize = itrtmp.size();

            itrtmp.push(&_nodes[0]);
            do {
                const node *curr = static_cast<const node *>(itrtmp.top());
                if (curr->bound.isHit(r)) {
                    for (size_t i = 0; i < 2; i++)
                        if (curr->childIsLeaf[i]) {
                            const leaf &l = _leaves[curr->children[i]];
                            if (l.bound.isHit(r) && fn(*l.entity, r))
                                return true;
                        }
                        else
                            itrtmp.push(&_nodes[curr->children[i]]);
                }

                itrtmp.pop();
            } while (itrtmp.size() > emptySize);

            return false;
        }
    };
}  // namespace igi