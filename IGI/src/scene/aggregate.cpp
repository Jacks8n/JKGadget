#include "igiscene/aggregate.h"
#include "igigeometry/triangle.h"

/// This implementation of Spatial-Split BVH doesn't fully comply with the original idea
/// "Shrinking bounding box" procedure is omitted because it requires perplexing interface design

void igi::aggregate::initBuild(allocator_type tempAlloc) {
    class bounded_vector {
      protected:
        std::pmr::vector<leaf> _leaves;
        bound_t _bound;

      public:
        bounded_vector() { }
        explicit bounded_vector(const std::pmr::polymorphic_allocator<const leaf *> &alloc)
            : _leaves(alloc), _bound(bound_t::NegInf()) { }

        void add(const leaf &l) {
            _leaves.push_back(l);
            _bound.extend(l.bound);
        }

        const bound_t &getBound() const {
            return _bound;
        }

        const std::pmr::vector<leaf> &getLeaves() const {
            return _leaves;
        }

      protected:
        static void MoveTo(std::pmr::vector<leaf> &dest, std::pmr::vector<leaf> &src) {
            for (leaf &l : src)
                dest.push_back(std::move(l));
            src.clear();
        }
    };

    class bin : public bounded_vector {
        using bounded_vector::bounded_vector;

      public:
        bin() { }

        void moveTo(std::pmr::vector<leaf> &dest) {
            MoveTo(dest, _leaves);
        }
    };

    class split : public bounded_vector {
        std::pmr::vector<leaf> _leaves_left;
        std::pmr::vector<leaf> _leaves_right;

      public:
        split() { }
        split(const std::pmr::polymorphic_allocator<const leaf *> &alloc)
            : bounded_vector(alloc), _leaves_left(alloc), _leaves_right(alloc) { }

        void clearQueue() { _leaves.clear(); }

        void addLeft(const entity *e, const bound_t &b) { _leaves_left.emplace_back(e, b); }

        void addRight(const entity *e, const bound_t &b) { _leaves_right.emplace_back(e, b); }

        auto getLeftLeaves() { return std::make_pair(_leaves_left.begin(), _leaves_left.end()); }

        auto getRightLeaves() { return std::make_pair(_leaves_right.begin(), _leaves_right.end()); }

        void moveLeftTo(std::pmr::vector<leaf> &dest) { MoveTo(dest, _leaves_left); }

        void moveRightTo(std::pmr::vector<leaf> &dest) { MoveTo(dest, _leaves_right); }
    };

    class sah {
        bound_t _bound;
        single _sahCost;
        single _childSA;

      public:
        sah() : _bound(bound_t::NegInf()), _childSA(0_sg) { }
        sah(const sah &o) : _bound(o._bound), _childSA(o._childSA) { }

        const bound_t &getBound() const {
            return _bound;
        }

        void set(const bounded_vector &bv) {
            _bound   = bv.getBound();
            _childSA = 0_sg;
            accumChildSA(bv);
        }

        void extend(const bounded_vector &bv) {
            _bound.extend(bv.getBound());
            accumChildSA(bv);
        }

        single getSAHCost() const {
            return _sahCost;
        }

        single getSAHCostIfAdd(const bound_t &b, bound_t *res) const {
            bound_t tmp = b;
            tmp.extend(*res);

            single cost = (_childSA + GetBoundSA(b)) / GetBoundSA(tmp);

            // Guarantee that the result is same whether b and res refer to same address or not
            *res = tmp;
            return cost;
        }

      private:
        static single GetBoundSA(const bound_t &b) {
            vec3f d = b.getDiagonal();
            return d[0] * d[1] * d[2];
        }

        void accumChildSA(const bounded_vector &bv) {
            for (const leaf &l : bv.getLeaves())
                _childSA += GetBoundSA(l.bound);
            _sahCost = _childSA / GetBoundSA(_bound);
        }
    };

    using leaf_it     = typename std::pmr::vector<leaf>::iterator;
    using range_t     = std::pair<leaf_it, leaf_it>;
    using itr_range_t = std::pair<size_t, size_t>;

    constexpr size_t MaxBinCount = 16;
    constexpr single MinBinSize  = 1e-2_sg;

    constexpr auto getBinCount = [](const range_t &r, single interval) {
        size_t n(interval / MinBinSize);
        return n < 1 ? 1 : n > MaxBinCount ? MaxBinCount : n;
    };
    constexpr auto getBinIndex = [](single coord, single binSizeInv, size_t binCount) -> size_t {
        if (!(coord > 0_sg)) return 0;

        size_t i = static_cast<size_t>(coord * binSizeInv);
        return i < binCount ? i : binCount - 1;
    };

    std::pmr::vector<leaf> leaves(_leaves, tempAlloc);

    size_t nleaves = _leaves.size();
    size_t nbin    = nleaves - 1 > MaxBinCount ? MaxBinCount : nleaves - 1;

    std::pmr::vector<bin> bins(nbin, tempAlloc);
    std::pmr::vector<split> splits(nbin - 1, tempAlloc);
    std::pmr::vector<std::pair<sah, sah>> binBounds(nbin - 1, std::make_pair(sah(), sah()), tempAlloc);

    std::stack<itr_range_t, std::pmr::vector<itr_range_t>> iterations(tempAlloc);

    for (size_t i = 0; i < nbin - 1; i++) {
        bins.emplace_back(tempAlloc);
        splits.emplace_back(tempAlloc);
    }
    bins.emplace_back(tempAlloc);

    bound_t &bound_root = _nodes.emplace_back().bound = bound_t::NegInf();
    std::for_each(leaves.begin(), leaves.end(), [&](leaf &e) { bound_root.extend(e.bound); });

    auto currNode = _nodes.begin();
    iterations.emplace(0, nleaves);
    while (iterations.size()) {
        bound_t &bound_node   = currNode->bound;
        itr_range_t itr_range = iterations.top();
        iterations.pop();

        range_t curr(leaves.begin() + itr_range.first, leaves.begin() + itr_range.second);
        vec3f diag      = bound_node.getDiagonal();
        size_t maxDim   = MaxIcf(diag[0], diag[1], diag[2]);
        single interval = diag[maxDim];
        single min_node = bound_node.getMin(nbin = getBinCount(curr, interval));

        auto [loLeaf, hiLeaf] = curr;
        single binSize        = interval / nbin;
        single binSizeInv     = nbin / interval;
        do {
            auto [min_leaf, max_leaf] = loLeaf->bound.getInterval(maxDim);

            size_t lo_bin = getBinIndex(min_leaf - min_node, binSizeInv, nbin);
            size_t hi_bin = getBinIndex(max_leaf - min_node, binSizeInv, nbin);
            if (lo_bin == hi_bin)
                bins[lo_bin].add(*loLeaf);
            else
                while (lo_bin < hi_bin)
                    splits[lo_bin++].add(*loLeaf);
        } while (++loLeaf != hiLeaf);

        binBounds[0].first.set(bins[0]);
        binBounds[nbin - 1].second.set(bins[nbin - 1]);
        for (size_t i = 1, j = nbin - 2; i < nbin; i++, j--) {
            binBounds[i].first = binBounds[i - 1].first;
            binBounds[i].first.extend(bins[i]);

            binBounds[j].second = binBounds[j + 1].second;
            binBounds[j].second.extend(bins[j + 1]);
        }

        bound_t bound_left, bound_right, bound_splitLeft, bound_splitRight;
        single cost_min = SingleInf;
        single costs[3], &cost_left = costs[0], &cost_right = costs[1], &cost_split = costs[2];
        size_t minSplitI = 0;
        for (size_t i = 0; i < splits.size(); i++) {
            split &split   = splits[i];
            sah &bin_left  = binBounds[i].first;
            sah &bin_right = binBounds[i].second;

            const auto &splitLeaves = split.getLeaves();
            single splitCoord       = min_node + i * binSize;
            for (const leaf &l : splitLeaves) {
                cost_left = bin_left.getSAHCostIfAdd(l.bound, &bound_left)
                            + bin_right.getSAHCost();
                cost_right = bin_right.getSAHCostIfAdd(l.bound, &bound_right)
                             + bin_left.getSAHCost();

                bound_splitRight = bound_splitLeft = l.bound;
                bound_splitLeft.setMin(maxDim, splitCoord);
                bound_splitRight.setMin(maxDim, splitCoord);
                cost_split = bin_left.getSAHCostIfAdd(bound_splitLeft, &bound_splitLeft)
                             + bin_right.getSAHCostIfAdd(bound_splitRight, &bound_splitRight);

                switch (MinIcf(cost_left, cost_right, cost_split)) {
                    case 0: split.addLeft(l.entity, bound_left); break;
                    case 1: split.addRight(l.entity, bound_right); break;
                    case 2:
                        split.addLeft(l.entity, bound_splitLeft);
                        split.addRight(l.entity, bound_splitRight);
                        break;
                }
            }
            split.clearQueue();

            if (costs[MinIcf(cost_left, cost_right, cost_split)] < cost_min)
                minSplitI = i;
        }

        leaves.erase(curr.first, curr.second);

        auto moveBin = [&](bin &b) { b.moveTo(leaves); };
        auto nextItr = [&](size_t lo, size_t hi, size_t side) {
            if (lo + 1 == hi) {
                currNode->childIsLeaf[side] = true;
                currNode->children[side]    = _leaves.size();
                _leaves.push_back(std::move(leaves[lo]));
            }
            else {
                currNode->childIsLeaf[side] = false;
                currNode->children[side]    = _nodes.size();
                _nodes.emplace_back().bound = (side ? binBounds[minSplitI].second
                                                    : binBounds[minSplitI].first)
                                                  .getBound();
                iterations.emplace(lo, hi);
            }
        };

        size_t nextItrLo = leaves.size();
        std::for_each(bins.begin(), bins.begin() + minSplitI, moveBin);
        splits[minSplitI].moveLeftTo(leaves);

        size_t nextItrHi = leaves.size();
        nextItr(nextItrLo, nextItrHi, 0);

        splits[minSplitI].moveRightTo(leaves);
        std::for_each(bins.begin() + minSplitI + 1, bins.begin() + nbin, moveBin);
        nextItr(nextItrHi, leaves.size(), 1);
    }
}
