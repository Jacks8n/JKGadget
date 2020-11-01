#pragma once

#include <bit>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <type_traits>

namespace igi {
    template <typename T0, typename T1, typename T2>
    constexpr decltype(auto) Clamp(T0 &&lo, T1 &&hi, T2 &&val) {
        return val < hi ? lo < val ? val : lo : hi;
    }

    template <typename T>
    constexpr decltype(auto) Saturate(T &&val) {
        return Clamp(static_cast<T>(0), static_cast<T>(1), val);
    }

    /// @param val is expected to be within [0, 1]
    template <typename T0, typename T1, typename T2>
    constexpr decltype(auto) Lerp(T0 &&lo, T1 &&hi, T2 &&val) {
        return Clamp(lo, hi, (hi - lo) * val + lo);
    }

    template <typename T0, typename T1, typename T2>
    constexpr decltype(auto) Ratio(T0 &&lo, T1 &&hi, T2 &&val) {
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
    using precise_float_t = std::conditional_t<std::is_same_v<T, float>, double, std::conditional_t<std::is_same_v<T, double> && std::is_same_v<T, long double>, long double, T>>;

    template <typename T, typename TRes = std::remove_cvref_t<T>>
    constexpr TRes Sqrt(const T &val) {
        using precise_t = precise_float_t<std::remove_cvref_t<T>>;

        constexpr precise_t InitialFactor { .86003999f };

        struct impl {
            static constexpr precise_t iterate(const precise_t &val, const precise_t &cur, int n) {
                return n ? iterate(val, (cur * cur + val) / (cur * static_cast<precise_t>(2)), n - 1) : cur;
            };
        };

        return std::is_constant_evaluated() ? static_cast<TRes>(impl::iterate(val, InitialFactor * val, 32)) : std::sqrt(val);
    }

    template <typename T, typename TRes = std::remove_cvref_t<T>>
    constexpr bool Quadratic(T &&a, T &&b, T &&c, TRes *t0, TRes *t1) {
        using precise_t = precise_float_t<std::remove_cvref_t<T>>;

        constexpr auto IsNonZero = [](auto &&v) { return v > 0 || v < 0; };

        // degenerate to linear equation
        if (!IsNonZero(a)) {
            if (IsNonZero(b)) {
                *t0 = *t1 = -c / b;
                return true;
            }
            return false;
        }

        precise_t pa = a, pb = b, pc = c;
        precise_t d = pb * pb - pa * pc * 4;
        if (!(d >= 0))
            return false;

        precise_t sqrtd = Sqrt(d);
        precise_t q     = (b + (b < 0 ? -sqrtd : sqrtd)) * -.5;

        *t0 = q / a;
        *t1 = c / q;
        if (*t1 < *t0)
            std::swap(*t0, *t1);

        return true;
    }

    template <typename T>
    using float_bits_t = std::conditional_t<std::is_same_v<T, float>, uint32_t, std::conditional_t<std::is_same_v<T, double>, uint64_t, void>>;

    template <typename T>
    concept is_single_float_c = std::is_same_v<T, float> || std::is_same_v<T, double>;

    template <is_single_float_c T>
    constexpr T IncreaseBit(T val) {
        using bits_t = float_bits_t<T>;

        static_assert(std::numeric_limits<float>::is_iec559);
        if (val == std::numeric_limits<float>::infinity())
            return val;
        if (val == -static_cast<T>(0))
            val = static_cast<T>(0);

        bits_t bits = std::bit_cast<bits_t>(val) + (val >= static_cast<T>(0) ? 1 : -1);
        return std::bit_cast<T>(bits);
    }

    template <is_single_float_c T>
    constexpr T DecreaseBit(T val) {
        using bits_t = float_bits_t<T>;

        static_assert(std::numeric_limits<float>::is_iec559);
        if (val == -std::numeric_limits<float>::infinity())
            return val;
        if (val == static_cast<T>(0))
            val = -static_cast<T>(0);

        bits_t bits = std::bit_cast<bits_t>(val) + (val <= static_cast<T>(0) ? 1 : -1);
        return std::bit_cast<T>(bits);
    }
}  // namespace igi
