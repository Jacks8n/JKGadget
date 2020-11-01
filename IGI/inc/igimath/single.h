#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <compare>
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

    static_assert(std::numeric_limits<single>::has_infinity);
    constexpr single SingleInf = std::numeric_limits<single>::infinity();

    constexpr single SingleLarge = static_cast<single>(1) / SingleEpsilon;

    template <is_single_float_c T>
    class error_single {
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

        template <typename U>
        friend inline error_single<U> Sqrt(const error_single<U> &val);

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

      private:
        constexpr error_single(const T &val, const T &lower, const T &upper) : _value(val), _lowerBound(lower), _upperBound(upper) { }

      public:
        constexpr error_single &operator=(const error_single &) = default;
        constexpr error_single &operator=(error_single &&) = default;

        constexpr error_single &operator=(const T &val) {
            new (this) error_single(val);
            return *this;
        }

        constexpr const T *getValuePtr() const {
            return &_value;
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

        constexpr void check() const {
            std::is_constant_evaluated() ? (void)0 : assert(!std::isnan(_value) && !std::isnan(_lowerBound) && !std::isnan(_upperBound) && _lowerBound <= _value && _value <= _upperBound);
        }
    };

    template <is_single_float_c T>
    constexpr error_single<T> operator+(const error_single<T> &l, const T &r) {
        return error_single<T>::nextError(l._value + r, l._lowerBound + r, l._upperBound + r);
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator+(const T &l, const error_single<T> &r) {
        return operator+(r, l);
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator+(const error_single<T> &l, const error_single<T> &r) {
        return error_single<T>::nextError(l._value + r._value, l._lowerBound + r._lowerBound, l._upperBound + r._upperBound);
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator-(const error_single<T> &l, const T &r) {
        return error_single<T>::nextError(l._value - r, l._lowerBound - r, l._upperBound - r);
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator-(const T &l, const error_single<T> &r) {
        return operator-(r, l);
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator-(const error_single<T> &l, const error_single<T> &r) {
        return error_single<T>::nextError(l._value - r._value, l._lowerBound - r._upperBound, l._upperBound - r._lowerBound);
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator*(const error_single<T> &l, const T &r) {
        return r > 0 ? error_single<T>::nextError(l._value * r, l._lowerBound * r, l._upperBound * r)
                     : error_single<T>::nextError(l._value * r, l._upperBound * r, l._lowerBound * r);
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator*(const T &l, const error_single<T> &r) {
        return operator*(r, l);
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator*(const error_single<T> &l, const error_single<T> &r) {
        T ll = l._lowerBound * r._lowerBound;
        T lu = l._lowerBound * r._upperBound;
        T ul = l._upperBound * r._lowerBound;
        T uu = l._upperBound * r._upperBound;

        return error_single<T>::nextError(l._value * r._value,
                                          std::min(std::min(ll, lu), std::min(ul, uu)),
                                          std::max(std::max(ll, lu), std::max(ul, uu)));
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator/(const error_single<T> &l, const T &r) {
        return error_single<T>::nextError(l._value / r, l._lowerBound / r, l._upperBound / r);
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator/(const T &l, const error_single<T> &r) {
        T ub = l / r._upperBound;
        T lb = l / r._lowerBound;
        return error_single<T>::nextError(l / r._value, std::min(lb, ub), std::max(lb, ub));
    }

    template <is_single_float_c T>
    constexpr error_single<T> operator/(const error_single<T> &l, const error_single<T> &r) {
        T ll = l._lowerBound / r._lowerBound;
        T lu = l._lowerBound / r._upperBound;
        T ul = l._upperBound / r._lowerBound;
        T uu = l._upperBound / r._upperBound;

        return error_single<T>::nextError(l._value / r._value,
                                          std::min(std::min(ll, lu), std::min(ul, uu)),
                                          std::max(std::max(ll, lu), std::max(ul, uu)));
    }

    template <is_single_float_c T>
    constexpr std::weak_ordering operator<=>(const error_single<T> &l, const T &r) {
        return l._upperBound < r ? std::weak_ordering::less
                                 : r < l._lowerBound ? std::weak_ordering::greater
                                                     : std::weak_ordering::equivalent;
    }

    template <is_single_float_c T>
    constexpr std::weak_ordering operator<=>(const T &l, const error_single<T> &r) {
        return l < r._lowerBound ? std::weak_ordering::less
                                 : r._upperBound < l ? std::weak_ordering::greater
                                                     : std::weak_ordering::equivalent;
    }

    template <is_single_float_c T>
    constexpr std::weak_ordering operator<=>(const error_single<T> &l, const error_single<T> &r) {
        return l._upperBound < r._lowerBound ? std::weak_ordering::less
                                             : r._upperBound < l._lowerBound ? std::weak_ordering::greater
                                                                             : std::weak_ordering::equivalent;
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

    template <typename T>
    inline error_single<T> Sqrt(const error_single<T> &val) {
        return error_single<T>::nextError(std::sqrt(val._value), std::sqrt(val._lowerBound), std::sqrt(val._upperBound));
    }

    // The suffix of function name, i.e., .*(cf), indicates 'Conservative Floating Point'

    template <typename T0, typename T1, typename T2>
    constexpr size_t MaxIcf(T0 &&v0, T1 &&v1, T2 &&v2) {
        return v0 < v1 ? v1 < v2 ? 2 : 1
                       : v0 < v2 ? 2 : 0;
    }

    template <typename T0, typename T1, typename T2>
    constexpr size_t MinIcf(T0 &&v0, T1 &&v1, T2 &&v2) {
        return v0 < v1 ? v0 < v2 ? 0 : 2
                       : v1 < v2 ? 1 : 2;
    }

    template <typename T0, typename T1, typename T2>
    constexpr decltype(auto) Maxcf(T0 &&v0, T1 &&v1, T2 &&v2) {
        return v0 < v1 ? v1 < v2 ? v2 : v1
                       : v0 < v2 ? v2 : v0;
    }

    template <typename T0, typename T1, typename T2>
    constexpr decltype(auto) Mincf(T0 &&v0, T1 &&v1, T2 &&v2) {
        return v0 < v1 ? v0 < v2 ? v0 : v2
                       : v1 < v2 ? v1 : v2;
    }

    template <typename TLo, typename THi, typename TV>
    constexpr bool InRangecf(TLo &&lo, THi &&hi, TV &&v) {
        return lo < v && v < hi;
    }

    template <typename TLo0, typename THi0, typename TLo1, typename THi1>
    constexpr bool Overlapcf(TLo0 &&lo0, THi0 &&hi0, TLo1 &&lo1, THi1 &&hi1) {
        return !(hi0 < lo1 || hi1 < lo0);
    }
}  // namespace igi
