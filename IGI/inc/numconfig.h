#pragma once

namespace igi {

#ifdef NUMCONFIG_DOUBLE

using single = double;

constexpr single SingleEpsilon = 1e-10;

constexpr single SingleLarge = 1e10;

#else

using single = float;

constexpr single SingleEpsilon = 1e-5f;

constexpr single SingleLarge = 1e5f;

#endif


#pragma region Conservative Single Comparison

template<typename T>
constexpr single AsSingle(T val) {
    return static_cast<single>(val);
}

constexpr bool IsPositivecf(single v) { return v > SingleEpsilon; }

constexpr bool IsNegativecf(single v) { return v < -SingleEpsilon; }

constexpr bool Lesscf(single l, single r) { return l + SingleEpsilon < r; }

constexpr bool Greatercf(single l, single r) { return l > r + SingleEpsilon; }

constexpr bool Equalcf(single l, single r) {
    return l < r + SingleEpsilon && l + SingleEpsilon > r;
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

} // namespace igi
