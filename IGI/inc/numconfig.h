#pragma once

namespace igi {

#ifdef NUMCONFIG_DOUBLE

using single = double;

constexpr single SingleEpsilon = 1e-12f;

#else

using single = float;

constexpr single SingleEpsilon = 1e-5f;

#endif

constexpr single SingleMax = 1e200 * 1e200;

#pragma region Conservative Single Comparison

template<typename T>
inline constexpr single AsSingle(T val) {
    return static_cast<single>(val);
}

inline bool IsPositivecf(single v) { return v > SingleEpsilon; }

inline bool IsNegativecf(single v) { return v < -SingleEpsilon; }

inline bool Lesscf(single l, single r) { return l + SingleEpsilon < r; }

inline bool Greatercf(single l, single r) { return l > r + SingleEpsilon; }

inline bool Equalcf(single l, single r) {
    return l < r + SingleEpsilon && l + SingleEpsilon > r;
}

inline size_t Comparecf(single l, single r) {
    return Lesscf(l, r) ? -1 : Greatercf(l, r) ? 1 : 0;
}

inline bool InRangecf(single lo, single hi, single v) {
    return Greatercf(v, lo) && Lesscf(v, hi);
}

#pragma endregion

} // namespace igi