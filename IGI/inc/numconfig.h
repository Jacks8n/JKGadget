#pragma once

#include <utility>

namespace igi {

#ifdef IGI_DOUBLE_FLOAT

    using single = double;

    constexpr single SingleEpsilon = 1e-10;

    constexpr single SingleLarge = 1e10;

#else

    using single = float;

    constexpr single SingleEpsilon = 1e-5f;

    constexpr single SingleLarge = 1e5f;

#endif

    constexpr single SingleInf = 1e200 * 1e200;

#pragma region Conservative Single Comparison

    template <typename T>
    constexpr single AsSingle(T val) {
        return static_cast<single>(val);
    }

    constexpr single operator""_sg(long double val) {
        return static_cast<single>(val);
    }

    constexpr single operator""_sg(unsigned long long val) {
        return static_cast<single>(val);
    }

    // PS: The suffix of function name, i.e. .*(cf), indicates Conservative Floating Point

    constexpr bool IsPoscf(single v) { return v > SingleEpsilon; }

    constexpr bool IsNegcf(single v) { return v < -SingleEpsilon; }

    constexpr bool NotZero(single v) { return v > SingleEpsilon || v < -SingleEpsilon; }

    constexpr bool Lesscf(single l, single r) { return l + SingleEpsilon < r; }

    constexpr bool Greatercf(single l, single r) { return l > r + SingleEpsilon; }

    constexpr bool Equalcf(single l, single r) {
        return l < r + SingleEpsilon && l + SingleEpsilon > r;
    }

    constexpr bool Mincf(single l, single r) {
        return Lesscf(l, r) ? l : r;
    }

    constexpr bool Maxcf(single l, single r) {
        return Lesscf(l, r) ? r : l;
    }

    constexpr std::pair<single, single> Ascendcf(single l, single r) {
        return Lesscf(l, r) ? std::make_pair(l, r) : std::make_pair(r, l);
    }

    constexpr size_t MaxIcf(single v0, single v1, single v2) {
        return Lesscf(v0, v1)
                   ? Lesscf(v1, v2) ? 2 : 1
                   : Lesscf(v0, v2) ? 2 : 0;
    }

    constexpr size_t MinIcf(single v0, single v1, single v2) {
        return Lesscf(v0, v1)
                   ? Lesscf(v0, v2) ? 0 : 2
                   : Lesscf(v1, v2) ? 1 : 2;
    }

    constexpr size_t Comparecf(single l, single r) {
        return Lesscf(l, r) ? -1 : Lesscf(r, l) ? 1 : 0;
    }

    constexpr bool InRangecf(single lo, single hi, single v) {
        return Lesscf(lo, v) && Lesscf(v, hi);
    }

    constexpr bool Overlapcf(single lo0, single hi0, single lo1, single hi1) {
        return !(Lesscf(hi0, lo1) || Lesscf(hi1, lo0));
    }

#pragma endregion

}  // namespace igi
