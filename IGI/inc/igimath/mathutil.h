#pragma once

#include <bit>
#include <cstdint>
#include <numeric>
#include <tuple>
#include <type_traits>
#include "igiutilities/igiassert.h"

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

    /// @brief there's potential overflow for unsigend integers
    template <typename T>
    requires(std::is_integral_v<T>) constexpr float IntToFloatAggressive(const T &val) {
        // conversion from signed integer is faster than unsigned one unless avx512 is enabled
        // e.g., _mm_cvtepi64_ps converts 2 64bit unsigned integers into 32bit floating points in 5 clocks
        return static_cast<float>(static_cast<std::make_signed_t<T>>(val));
    }

    template <typename T>
    requires(std::is_integral_v<T>) constexpr T Log2Ceil(T val) {
        igiassert(val > 0);
        if (val == 1)
            return 0;

        if constexpr (impl::is_ieee754float_compatible_v<T>)
            return ((std::bit_cast<uint32_t>(IntToFloatAggressive(val - 1)) >> 23) & 0xff) - 126;
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
    requires(std::is_integral_v<T>) constexpr T Exp2Ceil(T val) {
        if constexpr (impl::is_ieee754float_compatible_v<T>)
            return 1 << Log2Ceil(val);
        else {
            --val;
            for (size_t i = 0; i < sizeof(T) / 2 + 1; i++)
                val |= val >> (1 << i);
            return ++val;
        }
    }

    template <typename T>
    constexpr T Log2Floor(T val) {
        igiassert(val > 0);
        if (val == 1)
            return 0;

        if constexpr (impl::is_ieee754float_compatible_v<T>)
            return ((std::bit_cast<uint32_t>(IntToFloatAggressive(val)) >> 23) & 0xff) - 127;
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
    constexpr T Exp2Floor(T val) {
        if (val <= static_cast<T>(0))
            return static_cast<T>(1);

        if constexpr (impl::is_ieee754float_compatible_v<T>)
            return 1 << Log2Floor(val);
        else {
            val >>= 1;
            for (size_t i = 0; i < sizeof(T) / 2 + 1; i++)
                val |= val >> (1 << i);
            return ++val;
        }
    }

    template <typename T, typename TInt>
    constexpr T PowInt(T base, TInt pow) {
        igiassert((base > 0 && pow >= 0) || (base == 0 && pow != 0));
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
    constexpr std::remove_cvref_t<T> Abs(T &&val) {
        return std::is_constant_evaluated() ? val < 0 ? -val : val : std::abs(val);
    }

    template <typename T>
    using precise_float_t = std::conditional_t<std::is_same_v<T, float>, double, std::conditional_t<std::is_same_v<T, double> && std::is_same_v<T, long double>, long double, T>>;

    template <typename T, typename TRes = std::remove_cvref_t<T>>
    constexpr TRes Sqrt(T &&val) {
        using float_t   = std::remove_cvref_t<T>;
        using precise_t = precise_float_t<float_t>;

        struct impl {
            static constexpr precise_t iterate(const precise_t &val, const precise_t &cur, size_t n) {
                if (cur == 0)
                    return cur;
                return n ? iterate(val, (cur * cur + val) / (cur + cur), n - 1) : cur;
            };
        };

        return std::is_constant_evaluated() ? static_cast<TRes>(impl::iterate(static_cast<precise_t>(val), static_cast<precise_t>(val), 32))
                                            : static_cast<TRes>(std::sqrt(val));
    }

    template <typename T, typename TRes = std::remove_cvref_t<T>>
    constexpr std::tuple<bool, TRes, TRes> Quadratic(T &&a, T &&b, T &&c) {
        using precise_t = precise_float_t<std::remove_cvref_t<T>>;

        if (a == 0) {
            if (b != 0) {
                const TRes root = -c / b;
                return std::make_tuple(true, root, root);
            }
            return std::make_tuple(false, TRes(), TRes());
        }

        const precise_t pa = a, pb = b, pc = c;
        const precise_t d = pb * pb - pa * pc * static_cast<precise_t>(4);
        if (d < 0)
            return std::make_tuple(false, TRes(), TRes());

        const precise_t sqrtd = Sqrt(d);
        const precise_t q     = (b + (b < 0 ? -sqrtd : sqrtd)) * static_cast<precise_t>(-.5);

        TRes r0 = q / a, r1 = c / q;
        if (r1 < r0)
            std::swap(r0, r1);

        return std::make_tuple(true, r0, r1);
    }

    template <typename T>
    concept is_single_float_c = std::is_same_v<T, float> || std::is_same_v<T, double>;

    /// @brief unsigned integer types whose size is equal to the size of floating types
    template <is_single_float_c T>
    using float_bits_t = std::conditional_t<sizeof(T) == 4, uint32_t, std::conditional_t<sizeof(T) == 8, uint64_t, void>>;

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
