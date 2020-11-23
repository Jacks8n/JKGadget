#pragma once

#include <functional>
#include <memory_resource>
#include <stack>
#include "igiacceleration/circular_list.h"
#include "igicontext.h"
#include "igientity/entity.h"

namespace igi {
    class aggregate {
        struct node {
            bool childIsLeaf[2];
            size_t children[2];
            size_t nchildrenLeaves[2];
            bound_t bound;

            node() { }

            node(bool leftIsLeaf, size_t left, bool rightIsLeaf, size_t right, bound_t bound)
                : childIsLeaf { leftIsLeaf, rightIsLeaf }, children { left, right }, bound(bound) { }
        };

        struct leaf {
            const entity *entity;
            bound_t bound;

            leaf() { }

            constexpr leaf(const igi::entity &ep, bound_t b) : entity(&ep), bound(b) { }
        };

        union packed_leaf {
            packed_leaf(const leaf &l) : leaf(l) { }
            packed_leaf(size_t n) : nleaves(n) { }

            operator const leaf &() const {
                return leaf;
            }

            leaf leaf;
            size_t nleaves;
        };

        using build_itr_queue_t = circular_list<packed_leaf>;

      public:
        using initializer_list_t = std::initializer_list<const std::reference_wrapper<entity>>;
        using itr_stack_t        = std::stack<const void *, std::pmr::vector<const void *>>;

        aggregate(aggregate &&o) = default;

        template <typename TIt>
        aggregate(TIt &&entityIt, size_t n) {
            initBuild(std::forward<TIt>(entityIt), n);
        }

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

        void initBuild(build_itr_queue_t iterations);

        template <typename TIt>
        void initBuild(TIt &&entityIt, size_t n) {
            if (!n)
                return;

            new (&_leaves) std::pmr::vector<leaf>(n, context::GetTypedAllocator<leaf>());

            if (n < 3) {
                new (&_nodes) std::pmr::vector<node>(1, context::GetTypedAllocator<node>());
                new (_leaves.data()) leaf(*entityIt, (*entityIt).getBound());
                if (n == 1) {
                    new (_nodes.data()) node(true, 0, false, 0, _leaves[0].bound);
                    _nodes.front().nchildrenLeaves[0] = 1;
                }
                else if (n == 2) {
                    ++entityIt;
                    new (_leaves.data() + 1) leaf(*entityIt, (*entityIt).getBound());

                    bound_t b = _leaves[0].bound;
                    new (_nodes.data()) node(true, 0, true, 1, b.extend(_leaves[1].bound));
                    _nodes.front().nchildrenLeaves[0] = 1;
                    _nodes.front().nchildrenLeaves[1] = 1;
                }
                return;
            }

            new (&_nodes) std::pmr::vector<node>(n - 1, context::GetTypedAllocator<node>());
            _nodes.clear();

            build_itr_queue_t iterations(n * 2, context::GetTypedAllocator<packed_leaf, allocate_usage::temp>());
            iterations.resize(n + 1);

            iterations.front().nleaves = n;
            std::for_each(++iterations.begin(), iterations.end(), [&](packed_leaf &i) {
                new (&i.leaf) leaf(*entityIt, (*entityIt).getBound());
                ++entityIt;
            });

            initBuild(std::move(iterations));
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
                            const auto nleaves  = curr->nchildrenLeaves[i];
                            const auto leavesLo = curr->children[i];

                            for (size_t j = 0; j < nleaves; j++) {
                                const leaf &l = _leaves[j + leavesLo];

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
