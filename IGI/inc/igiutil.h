#pragma once

#include <tuple>
#include <utility>
#include <type_traits>

namespace igi {
    template<typename ...Ts>
    struct first_t;

    template<typename T, typename ...Ts>
    struct first_t<T, Ts...> {
        using type = T;
    };

    template<>
    struct first_t<> {
        using type = void;
    };

    template<size_t N, size_t ...Is>
    constexpr size_t getFirstInt() {
        return N;
    }

    template<size_t N, size_t ...Is>
    constexpr size_t getFirstInt(std::index_sequence<N, Is...>) {
        return N;
    }

    template<size_t N, size_t ...Is>
    constexpr size_t getNthInt() {
        return std::get<N>(std::make_tuple(Is...));
    }

    template<size_t N, size_t ...Is>
    constexpr size_t getNthInt(std::index_sequence<Is...>) {
        return getNthInt<N, Is...>();
    }

    template<size_t N, size_t ...Is>
    constexpr std::index_sequence<Is...> removeFirst(std::index_sequence<N, Is...>) {
        return std::index_sequence<Is...>();
    }

    // Pair of bool and size_t
    template<bool B, size_t I>
    struct cond_val {
        static constexpr bool cond = B;
        static constexpr size_t val = I;
    };

    template<typename ...Ts>
    constexpr std::tuple<Ts...> concatTrueVal(std::tuple<Ts...> c, std::tuple<>) {
        return c;
    }

    template<typename ...Tls, typename Tr0, typename ...Trs>
    constexpr auto concatTrueVal(std::tuple<Tls...> c, std::tuple<Tr0, Trs...>) {
        if constexpr (Tr0::cond)
            return concatTrueVal(std::tuple<Tls..., Tr0>(), std::tuple<Trs...>());
        else
            return concatTrueVal(c, std::tuple<Trs...>());
    }

    template<typename ...Ts>
    constexpr auto toIndexSeq(std::tuple<Ts...>) {
        return std::index_sequence<Ts::val...>();
    }

    template<size_t I, size_t ...Is>
    constexpr auto removeFromInts() {
        return toIndexSeq(concatTrueVal(std::tuple<>(), std::make_tuple(cond_val<I != Is, Is>()...)));
    }

    template<size_t I, size_t ...Is>
    constexpr auto removeFromInts(std::index_sequence<Is...>) {
        return removeFromInts<I, Is...>();
    }

    template<size_t N, size_t ...Is>
    constexpr auto removeNthInt(std::index_sequence<Is...> is) {
        return removeFromInts<getNthInt<N>(is), Is...>();
    }
}