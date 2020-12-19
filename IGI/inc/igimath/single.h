#pragma once

#include <cassert>
#include <cmath>
#include <compare>
#include <ostream>
#include <utility>
#include "igimath/mathutil.h"
#include "igiutilities/serialize.h"

namespace igi {
#ifdef IGI_DOUBLE_FLOAT
    using single = double;
#else
    using single = float;
#endif

    constexpr single SingleEpsilon = std::numeric_limits<single>::epsilon();

    constexpr single SingleInf = std::numeric_limits<single>::infinity();

    constexpr single SingleLarge = static_cast<single>(1 << 15);

    /// @brief ordering is not transitive, i.e., a == b && b == c doesn't imply a == c, though the comparison is implemented as weak_ordering
    template <is_single_float_c T>
    class error_single {
        template <is_single_float_c U>
        friend class error_single;

        template <is_single_float_c U>
        friend constexpr error_single<U> operator+(const error_single<U> &l, const U &r);
        template <is_single_float_c U>
        friend constexpr error_single<U> operator+(const U &l, const error_single<U> &r);
        template <is_single_float_c U>
        friend constexpr error_single<U> operator+(const error_single<U> &l, const error_single<U> &r);

        template <is_single_float_c U>
        friend constexpr error_single<U> operator-(const error_single<U> &l, const U &r);
        template <is_single_float_c U>
        friend constexpr error_single<U> operator-(const U &l, const error_single<U> &r);
        template <is_single_float_c U>
        friend constexpr error_single<U> operator-(const error_single<U> &l, const error_single<U> &r);

        template <is_single_float_c U>
        friend constexpr error_single<U> operator*(const error_single<U> &l, const U &r);
        template <is_single_float_c U>
        friend constexpr error_single<U> operator*(const U &l, const error_single<U> &r);
        template <is_single_float_c U>
        friend constexpr error_single<U> operator*(const error_single<U> &l, const error_single<U> &r);

        template <is_single_float_c U>
        friend constexpr error_single<U> operator/(const error_single<U> &l, const U &r);
        template <is_single_float_c U>
        friend constexpr error_single<U> operator/(const U &l, const error_single<U> &r);
        template <is_single_float_c U>
        friend constexpr error_single<U> operator/(const error_single<U> &l, const error_single<U> &r);

        template <is_single_float_c U>
        friend constexpr std::weak_ordering operator<=>(const error_single<U> &l, const U &r);
        template <is_single_float_c U>
        friend constexpr std::weak_ordering operator<=>(const U &l, const error_single<U> &r);
        template <is_single_float_c U>
        friend constexpr std::weak_ordering operator<=>(const error_single<U> &l, const error_single<U> &r);

        template <is_single_float_c U>
        friend inline decltype(auto) operator<<(std::ostream &os, const error_single<U> &val);

        using bits_t = float_bits_t<T>;

        static constexpr bits_t FractionBitCount = std::is_same_v<T, float>    ? 23
                                                   : std::is_same_v<T, double> ? 52
                                                                               : 0;

        static constexpr bits_t ExponentBitMask = std::is_same_v<T, float>    ? (static_cast<bits_t>(0xff) << 23)
                                                  : std::is_same_v<T, double> ? (static_cast<bits_t>(0x7ff) << 52)
                                                                              : 0;

        T _value, _lowerBound, _upperBound;

      public:
        META_BE(error_single, rflite::func_a([](const serializer_t &ser) {
                    T val = serialization::Deserialize<T>(ser);
                    return rflite::meta_helper::any_ins<error_single>(val);
                }))

        constexpr error_single()                     = default;
        constexpr error_single(const error_single &) = default;
        constexpr error_single(error_single &&)      = default;

        constexpr error_single(const T &val) : _value(val), _lowerBound(val), _upperBound(val) { }

        template <typename U>
        constexpr error_single(const error_single<U> &val) : _value(val._value), _lowerBound(val._lowerBound), _upperBound(val._upperBound) { }

      private:
        constexpr error_single(const T &val, const T &lower, const T &upper) : _value(val), _lowerBound(lower), _upperBound(upper) { }

      public:
        constexpr error_single &operator=(const error_single &) = default;
        constexpr error_single &operator=(error_single &&) = default;

        constexpr error_single &operator=(const T &val) {
            new (this) error_single(val);
            return *this;
        }

        constexpr const T &getValue() const {
            return _value;
        }

        constexpr const T &getLowerBound() const {
            return _lowerBound;
        }

        constexpr const T &getUpperBound() const {
            return _upperBound;
        }

        constexpr std::pair<T, T> getBound() const {
            return std::make_pair(getLowerBound(), getUpperBound());
        }

        error_single sqrt() const {
            return nextError(std::sqrt(_value), std::sqrt(_lowerBound), std::sqrt(_upperBound));
        }

        constexpr operator const T &() const {
            return getValue();
        }

        constexpr operator std::tuple<const T &, const T &, const T &>() const {
            return std::forward_as_tuple(getValue(), getLowerBound(), getUpperBound());
        }

        constexpr error_single operator-() const {
            return error_single(-_value, -_upperBound, -_lowerBound);
        }

        error_single &operator+=(const T &r) {
            return *this = operator+(*this, r);
        }

        error_single &operator+=(const error_single &r) {
            return *this = operator+(*this, r);
        }

        error_single &operator-=(const T &r) {
            return *this = operator-(*this, r);
        }

        error_single &operator-=(const error_single &r) {
            return *this = operator-(*this, r);
        }

        error_single &operator*=(const T &r) {
            return *this = operator*(*this, r);
        }

        error_single &operator*=(const error_single &r) {
            return *this = operator*(*this, r);
        }

        error_single &operator/=(const T &r) {
            return *this = operator/(*this, r);
        }

        error_single &operator/=(const error_single &r) {
            return *this = operator/(*this, r);
        }

      private:
        static constexpr error_single nextError(const T &val, const T &lower, const T &upper) {
            error_single res(val, DecreaseBit(lower), IncreaseBit(upper));
            res.check();
            return res;
        }

        template <bool Ceil>
        static constexpr T correct(const T &val) {
            constexpr std::float_round_style round = std::numeric_limits<T>::round_style;

            if constexpr (round == std::float_round_style::round_toward_infinity)
                return Ceil ? val : DecreaseBit(val);
            else if constexpr (round == std::float_round_style::round_toward_neg_infinity)
                return Ceil ? IncreaseBit(val) : val;
            else
                return Ceil ? IncreaseBit(val) : DecreaseBit(val);
        }

        template <bool Ceil>
        static constexpr T addConservative(const T &l, const T &r) {
            const T res = l + r;

            return res - r != l ? correct<Ceil>(res) : res;
        }

        template <bool Ceil>
        static constexpr T subConservative(const T &l, const T &r) {
            const T res = l - r;
            return res + r != l ? correct<Ceil>(res) : res;
        }

        static constexpr bits_t floatToBits(const T &v) {
            return std::bit_cast<bits_t>(v) | (static_cast<bits_t>(1) << FractionBitCount);
        }

        static constexpr std::pair<bool, bool> testMulRound(const T &l, const T &r) {
            constexpr bits_t roundBit  = static_cast<bits_t>(1) << FractionBitCount;
            constexpr bits_t truncMask = (roundBit << 1) - 1;
            const bits_t fraction      = floatToBits(l) * floatToBits(r);
            const bits_t trunc         = fraction & truncMask;

            return std::make_pair(static_cast<bool>(trunc), static_cast<bool>(trunc & roundBit));
        }

        template <bool Ceil>
        static constexpr T mulConservative(const T &l, const T &r) {
            static_assert(std::numeric_limits<T>::is_iec559);

            const T res = l * r;
            if (res == 0)
                return res;

            const auto [trunc, ceil] = testMulRound(l, r);
            if (trunc)
                if constexpr (std::numeric_limits<T>::round_style == std::float_round_style::round_to_nearest)
                    if constexpr (Ceil)
                        return ceil ^ (res > 0) ? IncreaseBit(res) : res;
                    else
                        return ceil ^ (res > 0) ? res : DecreaseBit(res);
                else
                    return correct<Ceil>(res);
            else
                return res;
        }

        template <bool Ceil>
        static constexpr T divConservative(const T &l, const T &r) {
            T res = l / r;

            const auto [trunc, ceil] = testMulRound(res, r);
            if (trunc) {
                if constexpr (std::numeric_limits<T>::round_style == std::float_round_style::round_to_nearest)
                    if constexpr (Ceil)
                        return ceil ^ (res < 0) ? IncreaseBit(res) : res;
                    else
                        return ceil ^ (res < 0) ? res : DecreaseBit(res);
                else
                    return correct<Ceil>(res);
            }
            return res;
        }

        constexpr void check() const {
            igiassert(!std::isnan(_value) && !std::isnan(_lowerBound) && !std::isnan(_upperBound) && _lowerBound <= _value && _value <= _upperBound);
        }
    };

    template <is_single_float_c T>
    constexpr error_single<T> operator+(const error_single<T> &l, const T &r) {
        return error_single<T>(l._value + r,
                               error_single<T>::template addConservative<false>(l._lowerBound, r),
                               error_single<T>::template addConservative<true>(l._upperBound, r));
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator+(const T &l, const error_single<T> &r) {
        return operator+(r, l);
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator+(const error_single<T> &l, const error_single<T> &r) {
        return error_single<T>(l._value + r._value,
                               error_single<T>::template addConservative<false>(l._lowerBound, r._lowerBound),
                               error_single<T>::template addConservative<true>(l._upperBound, r._upperBound));
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator-(const error_single<T> &l, const T &r) {
        return error_single<T>(l._value - r,
                               error_single<T>::template subConservative<false>(l._lowerBound, r),
                               error_single<T>::template subConservative<true>(l._upperBound, r));
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator-(const T &l, const error_single<T> &r) {
        return error_single<T>(l - r._value,
                               error_single<T>::template subConservative<false>(l, r._upperBound),
                               error_single<T>::template subConservative<true>(l, r._lowerBound));
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator-(const error_single<T> &l, const error_single<T> &r) {
        return error_single<T>(l._value - r._value,
                               error_single<T>::template subConservative<false>(l._lowerBound, r._upperBound),
                               error_single<T>::template subConservative<true>(l._upperBound, r._lowerBound));
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator*(const error_single<T> &l, const T &r) {
        return r >= 0 ? error_single<T>(l._value * r,
                                        error_single<T>::template mulConservative<false>(l._lowerBound, r),
                                        error_single<T>::template mulConservative<true>(l._upperBound, r))
                      : error_single<T>(l._value * r,
                                        error_single<T>::template mulConservative<false>(l._upperBound, r),
                                        error_single<T>::template mulConservative<true>(l._lowerBound, r));
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator*(const T &l, const error_single<T> &r) {
        return operator*(r, l);
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator*(const error_single<T> &l, const error_single<T> &r) {
        const error_single<T> lb = l * r._lowerBound;
        const error_single<T> ub = l * r._upperBound;
        return error_single<T>(lb._value,
                               std::min(lb._lowerBound, ub._lowerBound),
                               std::max(lb._upperBound, ub._upperBound));
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator/(const error_single<T> &l, const T &r) {
        return r > 0 ? error_single<T>(l._value / r,
                                       error_single<T>::template divConservative<false>(l._lowerBound, r),
                                       error_single<T>::template divConservative<true>(l._upperBound, r))
                     : error_single<T>(l._value / r,
                                       error_single<T>::template divConservative<false>(l._upperBound, r),
                                       error_single<T>::template divConservative<true>(l._lowerBound, r));
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator/(const T &l, const error_single<T> &r) {
        T lb = r._lowerBound, ub = r._upperBound;

        if ((l > 0) ^ (r._lowerBound < 0 && 0 < r._upperBound))
            std::swap(lb, ub);

        return error_single<T>(l / r._value,
                               error_single<T>::template divConservative<false>(l, lb),
                               error_single<T>::template divConservative<true>(l, ub));
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator/(const error_single<T> &l, const error_single<T> &r) {
        igiassert(!(r._lowerBound <= 0 && 0 <= r._upperBound));

        T v[4];
        if (0 < r._lowerBound)
            v[0] = l._lowerBound, v[2] = l._upperBound;
        else
            v[0] = l._upperBound, v[2] = l._lowerBound;

        if (0 < l._lowerBound)
            v[1] = r._upperBound, v[3] = r._lowerBound;
        else if (l._upperBound < 0)
            v[1] = r._lowerBound, v[3] = r._upperBound;
        else
            v[1] = v[3] = 0 < r._lowerBound ? r._upperBound : r._lowerBound;

        return error_single<T>(l._value / r._value,
                               error_single<T>::template divConservative<false>(v[0], v[1]),
                               error_single<T>::template divConservative<true>(v[2], v[3]));
    }

    template <is_single_float_c T>
    constexpr std::weak_ordering operator<=>(const error_single<T> &l, const T &r) {
        return l._upperBound < r   ? std::weak_ordering::less
               : r < l._lowerBound ? std::weak_ordering::greater
                                   : std::weak_ordering::equivalent;
    }

    template <is_single_float_c T>
    constexpr std::weak_ordering operator<=>(const T &l, const error_single<T> &r) {
        return l < r._lowerBound   ? std::weak_ordering::less
               : r._upperBound < l ? std::weak_ordering::greater
                                   : std::weak_ordering::equivalent;
    }

    template <is_single_float_c T>
    constexpr std::weak_ordering operator<=>(const error_single<T> &l, const error_single<T> &r) {
        return l._upperBound < r._lowerBound   ? std::weak_ordering::less
               : r._upperBound < l._lowerBound ? std::weak_ordering::greater
                                               : std::weak_ordering::equivalent;
    }

    template <is_single_float_c T>
    constexpr bool operator==(const T &l, const error_single<T> &r) {
        return l <= r && r <= l;
    }

    template <is_single_float_c T>
    constexpr bool operator==(const error_single<T> &l, const T &r) {
        return l <= r && r <= l;
    }

    template <is_single_float_c T>
    constexpr bool operator==(const error_single<T> &l, const error_single<T> &r) {
        return l <= r && r <= l;
    }

    template <is_single_float_c T>
    constexpr bool operator!=(const T &l, const error_single<T> &r) {
        return !operator==(l, r);
    }

    template <is_single_float_c T>
    constexpr bool operator!=(const error_single<T> &l, const T &r) {
        return !operator==(l, r);
    }

    template <is_single_float_c T>
    constexpr bool operator!=(const error_single<T> &l, const error_single<T> &r) {
        return !operator==(l, r);
    }

    template <is_single_float_c T>
    inline decltype(auto) operator<<(std::ostream &os, const error_single<T> &val) {
        return os << "{ " << val._lowerBound << ", " << val._value << ", " << val._upperBound << " }";
    }

    using esingle = error_single<single>;

    constexpr single operator""_sg(long double val) {
        return single(val);
    }

    constexpr single operator""_sg(unsigned long long val) {
        return single(val);
    }

    constexpr esingle operator""_es(long double val) {
        return esingle(val);
    }

    constexpr esingle operator""_es(unsigned long long val) {
        return esingle(val);
    }

    // The suffix of function name, i.e., .*(cf), indicates 'Conservative Floating Point'

    template <typename T0, typename T1, typename T2>
    constexpr size_t MaxIcf(T0 &&v0, T1 &&v1, T2 &&v2) {
        return v0 < v1   ? v1 < v2 ? 2 : 1
               : v0 < v2 ? 2
                         : 0;
    }

    template <typename T0, typename T1, typename T2>
    constexpr size_t MinIcf(T0 &&v0, T1 &&v1, T2 &&v2) {
        return v0 < v1   ? v0 < v2 ? 0 : 2
               : v1 < v2 ? 1
                         : 2;
    }

    template <typename T0, typename T1, typename T2>
    constexpr decltype(auto) Maxcf(T0 &&v0, T1 &&v1, T2 &&v2) {
        return v0 < v1   ? v1 < v2 ? v2 : v1
               : v0 < v2 ? v2
                         : v0;
    }

    template <typename T0, typename T1, typename T2>
    constexpr decltype(auto) Mincf(T0 &&v0, T1 &&v1, T2 &&v2) {
        return v0 < v1   ? v0 < v2 ? v0 : v2
               : v1 < v2 ? v1
                         : v2;
    }

    template <typename TLo, typename THi, typename TV>
    constexpr bool InRangecf(TLo &&lo, THi &&hi, TV &&v) {
        return lo < v && v < hi;
    }

    template <typename TLo, typename THi, typename TV>
    constexpr bool InRangeClosecf(TLo &&lo, THi &&hi, TV &&v) {
        return lo <= v && v <= hi;
    }

    template <typename TLo0, typename THi0, typename TLo1, typename THi1>
    constexpr bool Overlapcf(TLo0 &&lo0, THi0 &&hi0, TLo1 &&lo1, THi1 &&hi1) {
        return !(hi0 < lo1 || hi1 < lo0);
    }
}  // namespace igi
