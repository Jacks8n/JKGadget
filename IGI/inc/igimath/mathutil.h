#pragma once

#include <type_traits>

namespace igi {
    template <typename T>
    constexpr T Clamp(T lo, T hi, T val) {
        return val < lo ? lo : val > hi ? hi : val;
    }

    template <typename T>
    constexpr T Saturate(T val) {
        return Clamp(static_cast<T>(0), static_cast<T>(1), val);
    }

    template <typename T0, typename TUVIt>
    constexpr T0 Lerp(T0 lo, T0 hi, TUVIt val) {
        return (hi - lo) * val + lo;
    }

    template <typename T>
    constexpr T Ratio(T lo, T hi, T val) {
        return (val - lo) / (hi - lo);
    }

    template <typename T>
    T CeilExp2(T val) {
        size_t s = sizeof(T) * 2;
        size_t e = static_cast<T>(1) << (sizeof(T) * 4);
        do {
            e = e > val ? e >> s : e << s;
            s /= 2;
        } while (s);
        return e < val ? e << 1 : e;
    }

    template <typename T>
    T CeilLog2(T val) {
        val++;

        size_t s = sizeof(T) * 2;
        size_t l = 0;
        do {
            if (val >> s) {
                val += s;
                l += s;
            }
            s /= 2;
        } while (s);
        return l;
    }
    
    template <typename T>
    constexpr T SqrtConstexpr(const T &val) {
        constexpr T InitialFactor = .86003999;

        if constexpr (std::is_constant_evaluated()) {
            struct impl {
                static constexpr T iterate(const T &val, const T &cur, int n) {
                    return n > 0 ? iterate(val, (cur * cur + val) / (cur * static_cast<T>(2)), n - 1)
                                 : cur;
                };
            };
            return impl::iterate(val, InitialFactor * val, 32);
        }
        else
            return sqrt(val);
    }
}  // namespace igi
