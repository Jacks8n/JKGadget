#pragma once

#include "igimath/vec.h"

namespace igi {
    template <size_t N, typename T = size_t>
    requires(std::is_unsigned_v<T>) class mcode {
      public:
        using coord_t = vec<T, N>;
        using index_t = T;

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

        static constexpr index_t ToSpaced(const index_t &i) {
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
}  // namespace igi
