#include "igiscene/aggregate.h"
#include <queue>
#include "igigeometry/triangle.h"

/// This implementation of Spatial-Split BVH doesn't fully comply with the original idea
/// "Shrinking bounding box" procedure is omitted because it requires perplexing interface design

void igi::aggregate::initBuild(allocator_type tempAlloc) {
    /// @brief vector with bound of its elements
    class bounded_vector {
      protected:
        std::pmr::vector<leaf> _leaves;
        bound_t _bound;

      public:
        bounded_vector() { }

        explicit bounded_vector(const std::pmr::polymorphic_allocator<const leaf *> &alloc)
            : _leaves(alloc), _bound(bound_t::NegInf()) { }

        void resetBound() {
            _bound = bound_t::NegInf();
        }

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
            resetBound();
        }
    };

    /// @brief stores leaves lying across the split
    class split : public bounded_vector {
        std::pmr::vector<leaf> _leavesLeft;
        std::pmr::vector<leaf> _leavesRight;

      public:
        split() { }
        split(const std::pmr::polymorphic_allocator<const leaf *> &alloc)
            : bounded_vector(alloc), _leavesLeft(alloc), _leavesRight(alloc) { }

        void clearCandidates() {
            _leaves.clear();
            resetBound();
        }

        void clearLR() {
            _leavesLeft.clear();
            _leavesRight.clear();
        }

        void addLeft(const entity *e, const bound_t &b) { _leavesLeft.emplace_back(e, b); }

        void addRight(const entity *e, const bound_t &b) { _leavesRight.emplace_back(e, b); }

        auto getLeftLeaves() { return std::make_pair(_leavesLeft.begin(), _leavesLeft.end()); }

        auto getRightLeaves() { return std::make_pair(_leavesRight.begin(), _leavesRight.end()); }

        void moveLeftTo(std::pmr::vector<leaf> &dest) { MoveTo(dest, _leavesLeft); }

        void moveRightTo(std::pmr::vector<leaf> &dest) { MoveTo(dest, _leavesRight); }
    };

    /// @brief helper class for estimating sah
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

        void extend(const bound_t &b) {
            _bound.extend(b);
            _childSA += GetBoundSA(b);
            calcSA();
        }

        void extend(const bounded_vector &bv) {
            _bound.extend(bv.getBound());
            accumChildSA(bv);
        }

        single getSAHCost() const {
            return _sahCost;
        }

        single getSAHCostIfAdd(const bound_t &b, bound_t *res) const {
            bound_t tmp = _bound;
            tmp.extend(b);

            single cost = (_childSA + GetBoundSA(b)) / GetBoundSA(tmp);

            *res = tmp;
            return cost;
        }

      private:
        static single GetBoundSA(const bound_t &b) {
            if (b.isSingular())
                return 0_sg;

            vec3f d = b.getDiagonal();
            return d[0] * d[1] + d[1] * d[2] + d[2] * d[0];
        }

        void calcSA() {
            // make sure at least one children is added
            _sahCost = _childSA == 0 ? SingleInf : _childSA / GetBoundSA(_bound);
        }

        void accumChildSA(const bounded_vector &bv) {
            for (const leaf &l : bv.getLeaves())
                _childSA += GetBoundSA(l.bound);
            calcSA();
        }
    };

    using itr_range_t = std::pair<size_t, size_t>;

    static constexpr size_t MaxBinCount = 16;
    static constexpr single MinBinSize  = 1e-2_sg;

    constexpr auto getBinCount = [&](single interval) {
        size_t n(interval * (1_sg / MinBinSize));
        return Clamp(1, MaxBinCount, n);
    };

    constexpr auto getBinIndex = [](single coord, single binSizeInv, int binCount) -> size_t {
        int i = static_cast<int>(coord * binSizeInv);
        return Clamp(0, binCount - 1, i);
    };

    std::pmr::vector<leaf> leaves(_leaves, tempAlloc);

    bin bins[MaxBinCount];
    split splits[MaxBinCount - 1];
    std::pair<sah, sah> binBounds[MaxBinCount - 1];

    std::queue<itr_range_t, std::pmr::deque<itr_range_t>> iterations(tempAlloc);

    std::for_each(std::begin(bins), std::end(bins), [&](bin &b) { new (&b) bin(tempAlloc); });
    std::for_each(std::begin(splits), std::end(splits), [&](split &s) { new (&s) split(tempAlloc); });

    {
        bound_t &boundRoot = _nodes.emplace_back().bound = leaves.front().bound;
        std::for_each(++leaves.begin(), leaves.end(), [&](leaf &e) { boundRoot.extend(e.bound); });
    }

    iterations.emplace(0, _leaves.size());

    size_t curr = 0;
    do {
        node &currNode = _nodes[curr];

        bound_t &currNodeBound = currNode.bound;
        auto [lo, hi]          = iterations.front();
        iterations.pop();

        const vec3f diag          = currNodeBound.getDiagonal();
        const size_t maxDim       = MaxIcf(diag[0], diag[1], diag[2]);
        const single interval     = diag[maxDim];
        const single minNodeCoord = currNodeBound.getMin(maxDim);

        const size_t nbin       = getBinCount(interval);
        const single binSize    = interval / nbin;
        const single binSizeInv = nbin / interval;

        // add leaves to a bin within which they lie or splits through which they cross
        auto loLeaf = leaves.begin() + lo;
        auto hiLeaf = leaves.begin() + hi;
        do {
            auto [minLeafCoord, maxLeafCoord] = loLeaf->bound.getInterval(maxDim);

            size_t loBin = getBinIndex(minLeafCoord - minNodeCoord, binSizeInv, nbin);
            size_t hiBin = getBinIndex(maxLeafCoord - minNodeCoord, binSizeInv, nbin);
            if (loBin == hiBin)
                bins[loBin].add(*loLeaf);
            else
                while (loBin < hiBin)
                    splits[loBin++].add(*loLeaf);
        } while (++loLeaf != hiLeaf);

        // calculate two bounds of children if each split is performed
        binBounds[0].first.set(bins[0]);
        binBounds[nbin - 2].second.set(bins[nbin - 1]);
        for (size_t i = 1, j = nbin - 3; i < nbin - 1; i++, j--) {
            (binBounds[i].first = binBounds[i - 1].first).extend(bins[i]);
            (binBounds[j].second = binBounds[j + 1].second).extend(bins[j]);
        }

        // estimate and find split yielding minimum cost
        bound_t boundLeft, boundRight, boundSplitLeft, boundSplitRight;
        single costMin = SingleInf;
        single costs[3], &costLeft = costs[0], &costRight = costs[1], &costSplit = costs[2];
        size_t minSplitI = ~0;
        for (size_t i = 0; i < std::size(splits); i++) {
            split &split  = splits[i];
            sah &binLeft  = binBounds[i].first;
            sah &binRight = binBounds[i].second;

            // estimate costs of different split strategies
            const auto &splitLeaves = split.getLeaves();
            const single splitCoord = minNodeCoord + i * binSize;
            for (const leaf &l : splitLeaves) {
                // costs if partition the unbroken leaf
                costLeft = binLeft.getSAHCostIfAdd(l.bound, &boundLeft)
                           + binRight.getSAHCost();
                assert(!std::isnan(costLeft));

                costRight = binRight.getSAHCostIfAdd(l.bound, &boundRight)
                            + binLeft.getSAHCost();
                assert(!std::isnan(costRight));

                // costs if split the leaf into two parts
                boundSplitRight = boundSplitLeft = l.bound;

                boundSplitLeft.setMax(maxDim, splitCoord);
                boundSplitRight.setMin(maxDim, splitCoord);

                costSplit = binLeft.getSAHCostIfAdd(boundSplitLeft, &boundSplitLeft)
                            + binRight.getSAHCostIfAdd(boundSplitRight, &boundSplitRight);
                assert(!std::isnan(costSplit));

                switch (MinIcf(costLeft, costRight, costSplit)) {
                    case 0:
                        split.addLeft(l.entity, boundLeft);
                        binLeft.extend(boundLeft);
                        break;
                    case 1:
                        split.addRight(l.entity, boundRight);
                        binRight.extend(boundRight);
                        break;
                    default:
                        split.addLeft(l.entity, boundSplitLeft);
                        split.addRight(l.entity, boundSplitRight);
                        binLeft.extend(boundSplitLeft);
                        binRight.extend(boundSplitRight);
                        break;
                }
            }
            split.clearCandidates();

            single curCost = Mincf(costLeft, costRight, costSplit);
            if (curCost < costMin) {
                if (minSplitI != ~0)
                    splits[minSplitI].clearLR();

                minSplitI = i;
                costMin   = curCost;
            }
            else
                split.clearLR();
        }
        assert(minSplitI != ~0);

        auto moveBin = [&](bin &b) {
            b.moveTo(leaves);
        };
        auto nextItr = [&](size_t lo, size_t hi, size_t side) {
            assert(lo < hi);

            if (lo + 1 == hi) {
                currNode.childIsLeaf[side] = true;
                currNode.children[side]    = _leaves.size();
                _leaves.push_back(leaves[lo]);
            }
            else {
                currNode.childIsLeaf[side]  = false;
                currNode.children[side]     = _nodes.size();
                _nodes.emplace_back().bound = (side ? binBounds[minSplitI].second
                                                    : binBounds[minSplitI].first)
                                                  .getBound();
                iterations.emplace(lo, hi);
            }
        };

        size_t nextItrLo = leaves.size();
        auto rightBinLo  = std::for_each_n(bins, minSplitI, moveBin);
        splits[minSplitI].moveLeftTo(leaves);

        size_t nextItrMid = leaves.size();
        nextItr(nextItrLo, nextItrMid, 0);

        std::for_each(rightBinLo, bins + nbin, moveBin);
        splits[minSplitI].moveRightTo(leaves);
        nextItr(nextItrMid, leaves.size(), 1);

        ++curr;
    } while (iterations.size());

    std::for_each(std::begin(bins), std::end(bins), [](bin &b) { b.~bin(); });
    std::for_each(std::begin(splits), std::end(splits), [](split &s) { s.~split(); });
}
