#pragma once

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

    template <typename T>
    concept IsSingleFloatC = std::is_same_v<T, float> || std::is_same_v<T, double>;

    template <IsSingleFloatC T = single>
    struct error_single {
      private:
        static constexpr T ErrorFactor = std::numeric_limits<T>::epsilon();

        static constexpr T ErrorLowerBoundFactor = static_cast<T>(1) - ErrorFactor;

        static constexpr T ErrorUpperBoundFactor = static_cast<T>(1) + ErrorFactor;

        T _value, _lowerBound, _upperBound;

      public:
        META_BE(error_single, rflite::func_a([](const serializer_t &ser) {
                    T val = serialization::Deserialize<T>(ser);
                    return rflite::meta_helper::any_ins<error_single>(val);
                }))

        constexpr error_single()                     = default;
        constexpr error_single(const error_single &) = default;
        constexpr error_single(error_single &&)      = default;

        // specified with `explicit`, since error estimation is spacially costly
        constexpr explicit error_single(const T &val) : _value(val), _lowerBound(val), _upperBound(val) { }

        constexpr error_single &operator=(const error_single &) = default;
        constexpr error_single &operator=(error_single &&) = default;

        constexpr error_single &operator=(const T &val) {
            new (this) error_single(val);
        }

      private:
        constexpr error_single(const T &val, const T &lower, const T &upper) : _value(val), _lowerBound(lower), _upperBound(upper) { }

      public:
        constexpr const T *getValuePtr() const {
            return &_value;
        }

        constexpr T getValue() const {
            return _value;
        }

        constexpr T getLowerBound() const {
            return _lowerBound * _value;
        }

        constexpr T getUpperBound() const {
            return _upperBound * _value;
        }

        constexpr std::pair<T, T> getBound() const {
            return std::make_pair(getLowerBound(), getUpperBound());
        }

        constexpr operator const T &() const {
            return _value;
        }

        constexpr error_single operator-() const {
            return error_single(-_value, -_upperBound, -_lowerBound);
        }

        constexpr error_single operator+(const T &r) const {
            return nextError(_value + r);
        }

        constexpr error_single operator+(const error_single &r) const {
            return operator+(r._value);
        }

        constexpr error_single operator-(const T &r) const {
            return nextError(_value - r);
        }

        constexpr error_single operator-(const error_single &r) const {
            return operator-(r._value);
        }

        constexpr error_single operator*(const T &r) const {
            return nextError(_value * r);
        }

        constexpr error_single operator*(const error_single &r) const {
            return operator*(r._value);
        }

        constexpr error_single operator/(const T &r) const {
            return nextError(_value / r);
        }

        constexpr error_single operator/(const error_single &r) const {
            return operator/(r._value);
        }

        error_single &operator+=(const T &r) {
            *this = nextError(_value + r);
            return *this;
        }

        error_single &operator+=(const error_single &r) {
            return operator+=(r._value);
        }

        error_single &operator-=(const T &r) {
            *this = nextError(_value - r);
            return *this;
        }

        error_single &operator-=(const error_single &r) {
            return operator-=(r._value);
        }

        error_single &operator*=(const T &r) {
            *this = nextError(_value * r);
            return *this;
        }

        error_single &operator*=(const error_single &r) {
            return operator*=(r._value);
        }

        error_single &operator/=(const T &r) {
            *this = nextError(_value / r);
            return *this;
        }

        error_single &operator/=(const error_single &r) {
            return operator/=(r._value);
        }

      private:
        constexpr error_single nextError(const T &val) const {
            return error_single(val, _lowerBound * ErrorLowerBoundFactor, _upperBound * ErrorUpperBoundFactor);
        }
    };

    using esingle = error_single<>;

    constexpr single operator""_sg(long double val) {
        return single(val);
    }

    constexpr single operator""_sg(unsigned long long val) {
        return single(val);
    }

    constexpr single operator""_es(long double val) {
        return esingle(val);
    }

    constexpr single operator""_es(unsigned long long val) {
        return esingle(val);
    }

    // The suffix of function name, i.e., .*(cf), indicates 'Conservative Floating Point'

    constexpr bool IsPoscf(const single &v) { return SingleEpsilon < v; }

    constexpr bool IsPoscf(const esingle &v) { return 0 < v.getValue(); }

    constexpr bool IsNegcf(const single &v) { return v < -SingleEpsilon; }

    constexpr bool IsNegcf(const esingle &v) { return v.getValue() < 0; }

    template <typename T>
    constexpr bool NotZerocf(T &&v) { return IsPoscf(v) || IsNegcf(v); }

    constexpr bool Lesscf(const single &l, const single &r) { return l + SingleEpsilon < r; }

    constexpr bool Lesscf(const esingle &l, const esingle &r) { return l.getUpperBound() < r.getLowerBound(); }

    template <typename TL, typename TR>
    constexpr bool Greatercf(TL &&l, TR &&r) { return Lesscf(r, l); }

    template <typename TL, typename TR>
    constexpr bool Equalcf(TL &&l, TR &&r) { return !(Lesscf(l, r) || Greatercf(l, r)); }

    template <typename TL, typename TR>
    constexpr std::pair<single, single> Ascendcf(TL &&l, TR &&r) {
        return Lesscf(l, r) ? std::make_pair(l, r) : std::make_pair(r, l);
    }

    template <typename T0, typename T1, typename T2>
    constexpr size_t MaxIcf(T0 &&v0, T1 &&v1, T2 &&v2) {
        return Lesscf(v0, v1) ? Lesscf(v1, v2) ? 2 : 1
                              : Lesscf(v0, v2) ? 2 : 0;
    }

    template <typename T0, typename T1, typename T2>
    constexpr size_t MinIcf(T0 &&v0, T1 &&v1, T2 &&v2) {
        return Lesscf(v0, v1) ? Lesscf(v0, v2) ? 0 : 2
                              : Lesscf(v1, v2) ? 1 : 2;
    }

    template <typename TL, typename TR>
    constexpr size_t Comparecf(TL &&l, TR &&r) {
        return Lesscf(l, r) ? -1 : Lesscf(r, l) ? 1 : 0;
    }

    template <typename TLo, typename THi, typename TV>
    constexpr size_t InRangecf(TLo &&lo, THi &&hi, TV &&v) {
        return Lesscf(lo, v) && Lesscf(v, hi);
    }

    template <typename TLo0, typename THi0, typename TLo1, typename THi1>
    constexpr bool Overlapcf(TLo0 &&lo0, THi0 &&hi0, TLo1 &&lo1, THi1 &&hi1) {
        return !(Lesscf(hi0, lo1) || Lesscf(hi1, lo0));
    }
}  // namespace igi
