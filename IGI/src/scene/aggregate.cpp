#include "igiscene/aggregate.h"
#include "igigeometry/triangle.h"
#include "igiutilities/igiassert.h"

/// This implementation of Spatial-Split BVH doesn't fully comply with the original idea
/// "Shrinking bounding box" procedure is omitted because it requires perplexing interface design

void igi::aggregate::initBuild(build_itr_queue_t iterations) {
    class sah {
        bound_t _bound;
        single _childSA, _boundSAInv;

      public:
        sah() : _bound(bound_t::NegInf()), _childSA(0_sg), _boundSAInv(SingleInf) { }

        void include(const bound_t &b) {
            igiassert(!b.isSingular());

            _bound.extend(b);
            _childSA += getSA(b);
        }

        void include(const sah &o) {
            _bound.extend(o._bound);
            _childSA += o._childSA;
            calculateSAInv();
        }

        void calculateSAInv() {
            _boundSAInv = 1_sg / getSA(_bound);
        }

        single getSAH() const {
            return _childSA == 0_sg ? SingleInf : _boundSAInv * _childSA;
        }

        const bound_t &getBound() const {
            return _bound;
        }

        void reset() {
            new (this) sah();
        }

        static single getSA(const bound_t &b) {
            vec3f size = b.getDiagonal();
            return size[0] * size[1] + size[1] * size[2] + size[2] * size[0];
        }
    };

    class split {
        std::pmr::vector<std::pair<leaf, bool>> _leaves;

      public:
        split() : _leaves(context::GetTypedAllocator<std::pair<leaf, bool>>()) { }

        void add(const leaf &l, bool bl = false) {
            _leaves.emplace_back(l, bl);
        }

        decltype(auto) begin() {
            return _leaves.begin();
        }

        decltype(auto) end() {
            return _leaves.end();
        }

        decltype(auto) size() const {
            return _leaves.size();
        }

        void clear() {
            _leaves.clear();
        }
    };

    static constexpr size_t MaxBinCount   = 8;
    static constexpr size_t MaxSplitCount = MaxBinCount - 1;
    static constexpr size_t BatchSize     = 4;
    static constexpr single MinSplitRatio = .01_sg;

    constexpr auto getBinIndex = [](single coord, single origin, single binSizeInv, size_t maxBinCount) {
        int i = static_cast<int>((coord - origin) * binSizeInv);
        return Clamp(0, maxBinCount - 1, i);
    };

    constexpr auto sahIfInclude = [](const sah &s, const bound_t &b) {
        sah res = s;
        res.include(b);
        res.calculateSAInv();
        return res;
    };

    auto setNodeChildLeaf = [&](size_t i, auto lo, size_t nleaves, size_t c) {
        node &n = _nodes[i];

        n.childIsLeaf[c]     = true;
        n.children[c]        = _leaves.size();
        n.nchildrenLeaves[c] = nleaves;

        _leaves.reserve(nleaves);
        std::copy_n(lo, nleaves, std::back_inserter(_leaves));
    };

    auto setNodeChildNode = [&](size_t i, const bound_t &b, size_t c) {
        node &n = _nodes[i];

        n.childIsLeaf[c] = false;
        n.children[c]    = _nodes.size();
        _nodes.emplace_back();
        _nodes.back().bound = b;
    };

    // throughout the implementation, "bin" is mentioned as a abstract conception, rather than shown as a type

    // sahBegins and sahEnds records leaves on right side and left side of splits
    // e.g., all of the leaves that are on the left side of a given split "end" on the left side of the split
    // thus, the leftmost and rightmost bins are accounted for only once
    // sahBegins: not accounted | .. | .. |       ..
    // sahEnds:        ..       | .. | .. | not accounted
    sah sahBegins[MaxSplitCount], sahEnds[MaxSplitCount];
    // split records leaves that are across the split for further cost evaluation
    split splits[MaxSplitCount];

    std::pmr::vector<leaf> tmpLeaves(context::GetTypedAllocator<leaf, allocate_usage::temp>());

    // prepare for recursion
    single splitSAThres;
    {
        _nodes.emplace_back();
        bound_t &rootBound = _nodes.front().bound = bound_t::NegInf();
        std::for_each(++iterations.begin(), iterations.end(), [&](packed_leaf &l) { rootBound.extend(l.leaf.bound); });

        splitSAThres = sah::getSA(rootBound) * MinSplitRatio;
    }

    size_t nodeIndex = 0;
    do {
        const size_t nleaves = iterations.front().nleaves;
        iterations.pop_front();
        igiassert(nleaves > 1);

        // caculate the numbers of bins and splits
        const size_t nbins   = nleaves > MaxBinCount ? MaxBinCount : nleaves;
        const size_t nsplits = nbins - 1;

        // calculate bound of current node
        bound_t &nodeBound = _nodes[nodeIndex].bound;
        igiassert(!nodeBound.isSingular());

        // determine whether attmpt to split or not
        const bool trySplit = sah::getSA(nodeBound) > splitSAThres;

        // select widest dimension of the bound
        const vec3f diagonal      = nodeBound.getDiagonal();
        const size_t maxDim       = MaxIcf(diagonal[0], diagonal[1], diagonal[2]);
        const single interval     = diagonal[maxDim];
        const single binSize      = interval / nbins;
        const single binSizeInv   = nbins / interval;
        const single nodeBoundMin = nodeBound.getMin(maxDim);

        // traverse over leaves with adding them to bins and splits
        std::for_each_n(iterations.begin(), nleaves, [&](packed_leaf &l) {
            const leaf &leaf         = l.leaf;
            const bound_t &leafBound = leaf.bound;

            const auto binLo = getBinIndex(leafBound.getMin(maxDim), nodeBoundMin, binSizeInv, nbins);
            const auto binHi = getBinIndex(leafBound.getMax(maxDim), nodeBoundMin, binSizeInv, nbins);
            igiassert(leafBound.getMin(maxDim) >= nodeBound.getMin(maxDim));
            igiassert(leafBound.getMax(maxDim) <= nodeBound.getMax(maxDim));
            igiassert(binLo <= binHi);
            igiassert(InRangeClosecf(0, nbins - 1, binLo));
            igiassert(InRangeClosecf(0, nbins - 1, binHi));

            if (binLo)
                sahBegins[binLo - 1].include(leafBound);
            if (binHi < nbins - 1)
                sahEnds[binHi].include(leafBound);

            for (size_t i = binLo; i < binHi; i++)
                splits[i].add(leaf);
        });

        // calculate bound of union of partial bounds by adding leaves to bins previously
        sahEnds[0].calculateSAInv();
        sahEnds[nsplits - 1].calculateSAInv();
        for (size_t i = 1; i < nsplits; i++) {
            sahEnds[i].include(sahEnds[i - 1]);
            sahBegins[nsplits - 1 - i].include(sahBegins[nsplits - i]);
        }

        // estimate sah of leaves lying across splits
        single minSAH         = SingleInf;
        size_t bestSplitIndex = ~0;
        {
            // stores new leaves by split
            std::pmr::vector<leaf> &leavesSplit = tmpLeaves;

            for (size_t i = 0; i < nsplits; i++) {
                split &split = splits[i];
                if (!split.size())
                    continue;

                sah &sahLeft  = sahEnds[i];
                sah &sahRight = sahBegins[i];

                const single splitCoord = (i + 1) * binSize + nodeBoundMin;

                single sahs[3];
                size_t sahIndex = ~0;
                for (auto &j : split) {
                    leaf &leaf         = j.first;
                    bound_t &leafBound = leaf.bound;
                    bool &leafSide     = j.second;

                    const sah left  = sahIfInclude(sahLeft, leaf.bound);
                    const sah right = sahIfInclude(sahRight, leaf.bound);

                    sahs[0] = left.getSAH() + sahRight.getSAH();
                    sahs[1] = right.getSAH() + sahLeft.getSAH();

                    // todo
                    // it simply split the bound without shrinking it to compactly fit the leaf
                    // more careful estimation is required
                    sah splitLeft, splitRight;
                    if (trySplit) {
                        const auto [tmpMin, tmpMax] = leafBound.getInterval(maxDim);

                        leafBound.setMax(maxDim, splitCoord);
                        splitLeft = sahIfInclude(sahLeft, leafBound);
                        leafBound.setMax(maxDim, tmpMax);

                        leafBound.setMin(maxDim, splitCoord);
                        splitRight = sahIfInclude(sahRight, leafBound);
                        leafBound.setMin(maxDim, tmpMin);

                        sahs[2] = splitLeft.getSAH() + splitRight.getSAH();
                    }
                    else
                        sahs[2] = 0_sg;

                    sahIndex = MinIcf(sahs[0], sahs[1], sahs[2]);
                    switch (sahIndex) {
                        case 0:
                            sahLeft  = left;
                            leafSide = false;
                            break;
                        case 1:
                            sahRight = right;
                            leafSide = true;
                            break;
                        default:
                            igiassert(trySplit);
                            leavesSplit.push_back(leaf);

                            sahLeft  = splitLeft;
                            leafSide = false;
                            leafBound.setMax(maxDim, splitCoord);

                            sahRight = splitRight;
                            leavesSplit.back().bound.setMin(maxDim, splitCoord);
                            break;
                    }
                }
                igiassert(sahIndex != ~0);

                if (minSAH > sahs[sahIndex]) {
                    bestSplitIndex = i;
                    minSAH         = sahs[sahIndex];

                    for (leaf &j : leavesSplit)
                        split.add(j, true);
                }

                leavesSplit.clear();
            }
        }
        igiassert(bestSplitIndex != ~0);

        // group leaves of left and right children nodes for next recursion
        {
            std::pmr::vector<leaf> &leavesRight = tmpLeaves;

            const single splitCoord = (bestSplitIndex + 1) * binSize + nodeBoundMin;

            // emplace the number of leaves on the left side
            const size_t leftChildrenIndex = iterations.size() - nleaves;
            iterations.emplace_back(0);

            size_t nleft = 0;
            for (size_t i = 0; i < nleaves; i++) {
                const leaf &leaf = iterations.front().leaf;
                iterations.pop_front();

                if (leaf.bound.getMax(maxDim) < splitCoord) {
                    iterations.emplace_back(leaf);
                    nleft++;
                }
                else if (splitCoord < leaf.bound.getMin(maxDim))
                    leavesRight.emplace_back(leaf);
            }

            split &bestSplit = splits[bestSplitIndex];
            for (auto &i : bestSplit) {
                if (i.second)
                    leavesRight.emplace_back(i.first);
                else {
                    iterations.emplace_back(i.first);
                    nleft++;
                }
            }

            // if there is only one leaf as child, store it directly
            igiassert(nleft);
            if (nleft < BatchSize) {
                setNodeChildLeaf(nodeIndex, iterations.end() - nleft, nleft, 0);
                iterations.pop_back(nleft + 1);
            }
            else {
                iterations[leftChildrenIndex].nleaves = nleft;

                setNodeChildNode(nodeIndex, sahEnds[bestSplitIndex].getBound(), 0);
            }

            igiassert(!leavesRight.empty());
            if (leavesRight.size() < BatchSize)
                setNodeChildLeaf(nodeIndex, leavesRight.begin(), leavesRight.size(), 1);
            else {
                iterations.emplace_back(leavesRight.size());
                for (const leaf &i : leavesRight)
                    iterations.emplace_back(i);

                setNodeChildNode(nodeIndex, sahBegins[bestSplitIndex].getBound(), 1);
            }

            leavesRight.clear();
        }

        // clear
        for (size_t i = 0; i < nsplits; i++) {
            splits[i].clear();
            sahBegins[i].reset();
            sahEnds[i].reset();
        }

        ++nodeIndex;
    } while (iterations.size());
}
