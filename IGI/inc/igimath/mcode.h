#pragma once

#include "igimath/vec.h"

namespace igi {
    template <size_t N, typename T = unsigned>
    class mvec {
      public:
        using coord_t = vec<T, N>;

      private:
        coord_t _coord;

      public:
        constexpr mvec() : _coord() { }

        constexpr mvec(const coord_t &coord) : _coord(coord) { }

        constexpr const coord_t &coord() const {
            return _coord;
        }

        constexpr mvec &operator++() {
            for (T &i : _coord)
                if (++i & 1)
                    break;
            return *this;
        }

        constexpr mvec operator++(int) {
            mvec temp = *this;
            operator++();
            return temp;
        }

        constexpr operator const coord_t &() const {
            return coord();
        }

        constexpr bool operator!=(const mvec &r) const {
            return _coord != r._coord;
        }

        constexpr bool operator==(const mvec &r) const {
            return _coord == r._index;
        }
    };

    template <size_t N, typename T = unsigned>
    class mcurve;

    template <size_t N, typename T = unsigned>
    requires(std::is_unsigned_v<T>) class mcode {
        friend class mcurve<N, T>;

      public:
        using coord_t = typename mvec<N, T>::coord_t;
        using index_t = std::conditional_t<sizeof(T) >= 4, uint64_t, uint32_t>;

      private:
        static constexpr size_t MaxCoordBits  = sizeof(index_t) * 8 / N;
        static constexpr size_t MaxIndexBits  = MaxCoordBits * N;
        static constexpr size_t MaxShiftCount = Log2Ceil((MaxCoordBits - 1) * (N - 1));

        index_t _index;

      public:
        constexpr mcode() : _index(0) { }
        constexpr mcode(const coord_t &coord) : _index(ToIndex(coord)) { }

        template <size_t I>
        constexpr mcode &step() {
            static_assert(I < N);

            constexpr index_t mask = GetComponentMask<I>();

            index_t comp = _index | ~mask;
            comp ^= comp + 1;
            _index ^= comp & mask;
            return *this;
        }

        constexpr operator const index_t &() const {
            return _index;
        }

        constexpr bool operator!=(const mcode &r) const {
            return _index != r._index;
        }

        constexpr bool operator==(const mcode &r) const {
            return _index == r._index;
        }

      private:
        template <size_t I>
        static constexpr index_t GetSpacedMask() {
            if constexpr (I > 0) {
                constexpr size_t shift = 1 << (MaxShiftCount - I);

                index_t prev = GetSpacedMask<I - 1>();
                index_t res  = 0, last;
                for (size_t i = 0; i < MaxCoordBits; i++) {
                    last = ((prev - 1) ^ prev) & prev, prev ^= last;
                    res |= i * N < shift || last > 1 << (i * N - shift) ? last : last << shift;
                }
                return res;
            }
            else
                return static_cast<index_t>(~0) >> (sizeof(index_t) * 8 - MaxCoordBits);
        }

        static constexpr index_t ToSpaced(const T &i) {
            constexpr size_t shift = 1 << (MaxShiftCount - 1);

            return ([&]<size_t... Is>(std::index_sequence<Is...>) {
                index_t res = i;
                ((res = (res | (res << (shift >> Is))) & GetSpacedMask<Is + 1>()), ...);
                return res;
            })(std::make_index_sequence<MaxShiftCount>());
        }

        static constexpr index_t ToIndex(const coord_t &c) {
            return ([&]<size_t... Is>(std::index_sequence<Is...>) {
                return ((ToSpaced(c[Is]) << Is) | ...);
            })(std::make_index_sequence<N>());
        }

        template <size_t I>
        static constexpr index_t GetComponentMask() {
            static_assert(I < N);

            constexpr index_t bit = static_cast<index_t>(1) << I;

            return ([]<size_t... Is>(std::index_sequence<Is...>) {
                return ((bit << (Is * N)) | ...);
            })(std::make_index_sequence<MaxCoordBits>());
        }
    };

    template <size_t N, typename T>
    mcode(const vec<T, N> &) -> mcode<N, std::make_unsigned_t<T>>;

    template <size_t N, typename T>
    class mcurve {
        mcode<N, T> _index;
        mvec<N, T> _coord;

      public:
        using coord_t = typename mvec<N, T>::coord_t;
        using index_t = typename mcode<N, T>::index_t;

        constexpr mcurve() : _index(), _coord() { }

        constexpr mcurve(const coord_t &coord) : _index(coord), _coord(coord) { }

        constexpr mcurve &operator++() {
            ++_index._index;
            ++_coord;
            return *this;
        }

        constexpr mcurve operator++(int) {
            mcurve temp = *this;
            operator++();
            return temp;
        }

        constexpr operator const index_t &() const {
            return _index;
        }

        constexpr bool operator!=(const mcurve &r) const {
            return _index != r._index;
        }

        constexpr bool operator==(const mcurve &r) const {
            return _index == r._index;
        }
    };
}  // namespace igi
