#pragma once

namespace igi {
    template <typename T>
    constexpr T Clamp(T lo, T hi, T val) {
        return val < lo ? lo : val > hi ? hi : val;
    }

    template <typename T>
    constexpr T Saturate(T val) {
        return Clamp(static_cast<T>(0), static_cast<T>(1), val);
    }

    template <typename T0, typename T1>
    constexpr T0 Lerp(T0 lo, T0 hi, T1 val) {
        return (hi - lo) * val + lo;
    }

    template <typename T>
    constexpr T Ratio(T lo, T hi, T val) {
        return (val - lo) / (hi - lo);
    }
}  // namespace igi
