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

            constexpr leaf(const igi::entity &ep, bound_t b) : entity(&ep), bound(b) { }
        };

      public:
        using initializer_list_t = std::initializer_list<const std::reference_wrapper<entity>>;
        using allocator_type     = std::pmr::polymorphic_allocator<leaf>;
        using itr_stack_t        = std::stack<const void *, std::pmr::vector<const void *>>;

        aggregate(aggregate &&o, const allocator_type &alloc)
            : _nodes(std::move(o._nodes), alloc), _leaves(o._leaves, alloc) { }

        template <typename TIt>
        aggregate(TIt &&entityIt, size_t n, const allocator_type &alloc, const allocator_type &tempAlloc) {
            initBuild(std::forward<TIt>(entityIt), n, alloc, tempAlloc);
        }

        template <typename TIt>
        aggregate(TIt &&entityIt, size_t n, const allocator_type &alloc)
            : aggregate(std::forward<TIt>(entityIt), n, alloc, alloc) { }

        aggregate &operator=(const aggregate &) = delete;
        aggregate &operator=(aggregate &&) = delete;

        ~aggregate() = default;

        bool isHit(const ray &r, itr_stack_t &itrtmp) const {
            return hit_impl<true>(r, itrtmp,
                                  [](const entity &e, const ray &r) { return e.isHit(r); });
        }

        bool tryHit(ray &r, interaction *res, itr_stack_t &itrtmp) const {
            interaction tmp;
            tmp.entityId = interaction::EntityIDNull;

            bool hit = hit_impl<false>(r, itrtmp,
                                       [&](const entity &e, ray &r) {
                                           return interaction::EntityToID(&e) != tmp.entityId && e.tryHit(r, &tmp);
                                       });

            *res = tmp;
            return hit;
        }

      private:
        std::pmr::vector<node> _nodes;
        std::pmr::vector<leaf> _leaves;

        void initBuild(std::pmr::vector<leaf> &leaves, allocator_type tempAlloc);

        template <typename TIt>
        void initBuild(TIt &&entityIt, size_t n, const allocator_type &alloc, const allocator_type &tempAlloc) {
            if (!n)
                return;

            new (&_leaves) std::pmr::vector<leaf>(n, alloc);

            if (n < 3) {
                new (&_nodes) std::pmr::vector<node>(1, alloc);
                new (_leaves.data()) leaf(*entityIt, (*entityIt).getBound());
                if (n == 1)
                    _nodes.front() = node(true, 0, false, 0, _leaves[0].bound);
                else if (n == 2) {
                    ++entityIt;
                    new (_leaves.data() + 1) leaf(*entityIt, (*entityIt).getBound());

                    bound_t b      = _leaves[0].bound;
                    _nodes.front() = node(true, 0, true, 1, b.extend(_leaves[1].bound));
                }
                return;
            }

            std::pmr::vector<leaf> tmpLeaves(n, tempAlloc);

            for (size_t i = 0; i < n; i++, ++entityIt)
                new (tmpLeaves.data() + i) leaf(*entityIt, (*entityIt).getBound());

            initBuild(tmpLeaves, alloc);
        }

        template <bool FindFirst, typename TRay, typename TFn>
        bool hit_impl(TRay &&r, itr_stack_t &itrtmp, TFn &&fn) const {
            const size_t emptySize = itrtmp.size();

            bool hit = false;
            itrtmp.push(_nodes.data());
            do {
                const node *curr = static_cast<const node *>(itrtmp.top());
                itrtmp.pop();

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
            } while (itrtmp.size() > emptySize);

end:
            return hit;
        }
    };
}  // namespace igi
