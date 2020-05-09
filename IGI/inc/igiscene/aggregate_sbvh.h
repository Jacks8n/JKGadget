#pragma once

#include <stack>
#include "igiscene/IAggregate.h"
#include "igiscene/ISBVHItem.h"

namespace igi {
    template <typename T,
              std::enable_if_t<std::is_base_of_v<ISBVHItem, T>, size_t> = 0>
    class aggregate_sbvh : public IAggregate {
        struct node {
            bool childIsLeaf[2];
            size_t children[2];
            bound_t bound;
        };

      public:
        using allocator_type = std::pmr::polymorphic_allocator<aggregate_elem>;

        aggregate_sbvh(const allocator_type &alloc)
            : _nodes(alloc), _leaves(alloc) { }
        aggregate_sbvh(const aggregate_sbvh &o, const allocator_type &alloc)
            : _nodes(o._nodes, alloc), _leaves(o._leaves, alloc) { }
        aggregate_sbvh(aggregate_sbvh &&o, const allocator_type &alloc)
            : _nodes(std::move(o._nodes), alloc), _leaves(o._leaves, alloc) { }
        aggregate_sbvh(std::initializer_list<ISBVHItem *> il, const allocator_type &alloc)
            : aggregate_sbvh(il, alloc, alloc) { }
        aggregate_sbvh(std::initializer_list<ISBVHItem *> il, const allocator_type &alloc,
                       const allocator_type &tempAlloc)
            : _nodes(alloc), _leaves(il, alloc) {
            initBuild(tempAlloc);
        }

        aggregate_sbvh &operator=(const aggregate_sbvh &) = delete;
        aggregate_sbvh &operator=(aggregate_sbvh &&) = delete;

        ~aggregate_sbvh() = default;

        void add(const aggregate_elem &e) override;

        bool isHit(const ray &r) const override;

        bool tryHit(ray &r, interaction *res) const override;

      private:
        std::pmr::vector<node> _nodes;
        std::pmr::vector<aggregate_elem> _leaves;

        void initBuild(const allocator_type &alloc) {
            constexpr size_t MaxBinCount = 16;
            constexpr single MinBinSize  = 1e-2;

            if (!_leaves.size()) return;

            using range_t = std::pair<decltype(_leaves.begin()), decltype(_leaves.end())>;
            std::pmr::vector<range_t> range_stack(alloc);

            range_t range;

            bound_t boundWhole = bound_t::NegInf();
            for (aggregate_elem &e : _leaves)
                boundWhole.extend(e.bound);

            vec3f diag    = boundWhole.getDiagonal();
            size_t maxDim = MaxIcf(diag[0], diag[1], diag[2]);

            std::sort(_leaves.begin(), _leaves.end(),
                      [=](aggregate_elem &l, aggregate_elem &r) {
                          return Comparecf(l.bound.getInterval(maxDim),
                                           r.bound.getInterval(maxDim));
                      });

            using bin_bound_t = std::pair<bound_t, bound_t>;
            std::array<bin_bound_t, MaxBinCount> candidates(alloc);

            single interval = diag[maxDim];
            size_t binCount = Lesscf(interval, MaxBinCount * MinBinSize)
                                  ? Clamp(1, MaxBinCount, static_cast<size_t>(interval / MinBinSize))
                                  : MaxBinCount;

            candidates[0] = std::make_pair(range.first->bound, range.second[-1].bound);

            single split, binCountInv = AsSingle(1) / binCount;
            bound_t boundLeft, boundRight;
            auto [lo, hi] = range;
            for (size_t i = 1; i < binCount; i++) {
                split = interval * AsSingle(i) * binCountInv
                        + boundWhole.getMin()[maxDim];

                while (Lesscf(lo->bound.getMax()[maxDim], split) && lo != range.second)
                    candidates[i].first.extend(lo++->bound);
                while (!Lesscf(--hi->bound) && hi != range.first)
                    candidates[i].second.extend(hi);

                candidates[i] = candidates[i - 1];
                candidates[i].first.extend()
            }
        }
    };
}  // namespace igi