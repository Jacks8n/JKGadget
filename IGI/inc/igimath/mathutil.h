#pragma once

#include <bit>
#include <cassert>
#include <cstdint>
#include <numeric>
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

    namespace impl {
        /// @brief whether `T` can be safely converted to ieee754 floating point representation, of which exponent bits may be extracted
        template <typename T>
        constexpr bool is_ieee754float_compatible_v = std::numeric_limits<float>::is_iec559 && sizeof(T) < 32;
    }  // namespace impl

    template <typename T>
    constexpr T CeilLog2(T val) {
        assert(val > 0);
        if (val == 1)
            return 0;

        if constexpr (impl::is_ieee754float_compatible_v<T>) {
            float fval = static_cast<float>(val - 1);
            return ((std::bit_cast<uint32_t>(fval) >> 23) & 0xff) - 126;
        }
        else {
            ++val;

            T shift = sizeof(T) * 2, res = sizeof(T) * 4;
            while ((val >> res) != 1) {
                res += (val >> res) ? shift : -shift;
                shift /= 2;
            }
            return res;
        }
    }

    template <typename T>
    constexpr T CeilExp2(T val) {
        if constexpr (impl::is_ieee754float_compatible_v<T>)
            return 1 << CeilLog2(val);
        else {
            --val;
            for (size_t i = 0; i < sizeof(T) / 2 + 1; i++)
                val |= val >> (1 << i);
            return ++val;
        }
    }

    template <typename T>
    constexpr T FloorLog2(T val) {
        assert(val > 0);
        if (val == 1)
            return 0;

        if constexpr (impl::is_ieee754float_compatible_v<T>) {
            float fval = static_cast<float>(val);
            return ((std::bit_cast<uint32_t>(fval) >> 23) & 0xff) - 127;
        }
        else {
            T shift = sizeof(T) * 2, res = sizeof(T) * 4;
            while ((val >> res) != 1) {
                res += (val >> res) ? shift : -shift;
                shift /= 2;
            }
            return res;
        }
    }

    template <typename T>
    constexpr T FloorExp2(T val) {
        if constexpr (impl::is_ieee754float_compatible_v<T>)
            return 1 << FloorLog2(val);
        else {
            val >>= 1;
            for (size_t i = 0; i < sizeof(T) / 2 + 1; i++)
                val |= val >> (1 << i);
            return ++val;
        }
    }

    template <typename T, typename TInt>
    constexpr T PowInt(T base, TInt pow) {
        assert((base > 0 && pow >= 0) || (base == 0 && pow != 0));
        if (pow == 0)
            return 1;
        if (pow == 1 || base == 0 || base == 1)
            return base;

        TInt itr;
        T par, sum = 1;

        while (pow > 0) {
            itr = 1;
            par = base;
            while (itr * 2 < pow) {
                par *= par;
                itr *= 2;
            }
            pow -= itr;
            sum *= par;
        }

        return sum;
    }

    template <typename T>
    constexpr T SqrtConstexpr(const T &val) {
        constexpr T InitialFactor = .86003999;

        struct impl {
            static constexpr T iterate(const T &val, const T &cur, int n) {
                return n > 0 ? iterate(val, (cur * cur + val) / (cur * static_cast<T>(2)), n - 1)
                             : cur;
            };
        };

        return std::is_constant_evaluated() ? impl::iterate(val, InitialFactor * val, 32) : sqrt(val);
    }
}  // namespace igi
