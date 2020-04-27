#pragma once

namespace igi {
    template<typename T>
    constexpr auto Clamp(T lo, T hi, T val) {
        return val < lo ? lo : val > hi ? hi : val;
    }

    template<typename T>
    constexpr auto Saturate(T val) {
        return Clamp(static_cast<T>(0), static_cast<T>(1), val);
    }
}
