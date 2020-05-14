#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace igi {
    template <typename TTo, typename... TFroms>
    struct all_convertible {
        static constexpr bool value = (std::is_convertible_v<TFroms, TTo> && ...);
    };

    template <typename TTo, typename... TFroms>
    constexpr bool all_convertible_v = all_convertible<TTo, TFroms...>::value;

    template <size_t N, size_t... Is>
    constexpr size_t getFirstInt() {
        return N;
    }

    template <size_t N, size_t... Is>
    constexpr size_t getFirstInt(std::index_sequence<N, Is...>) {
        return N;
    }

    template <size_t N, size_t... Is>
    constexpr size_t getNthInt() {
        return std::get<N>(std::make_tuple(Is...));
    }

    template <size_t N, size_t... Is>
    constexpr size_t getNthInt(std::index_sequence<Is...>) {
        return getNthInt<N, Is...>();
    }

    template <size_t N, size_t... Is>
    constexpr std::index_sequence<Is...> removeFirst(std::index_sequence<N, Is...>) {
        return std::index_sequence<Is...>();
    }

    template <bool B, typename T>
    struct cond_type {
        static constexpr bool cond = B;
        using type                 = T;
    };

    // Pair of bool and size_t
    template <bool B, size_t I>
    struct cond_val {
        static constexpr bool cond = B;
        static constexpr auto val  = I;
    };

    template <typename... Ts>
    constexpr std::tuple<Ts...> concatTrueCond(std::tuple<Ts...> c, std::tuple<>) {
        return c;
    }

    template <typename... Tls, typename Tr0, typename... Trs>
    constexpr auto concatTrueCond(std::tuple<Tls...> c, std::tuple<Tr0, Trs...>) {
        if constexpr (Tr0::cond)
            return concatTrueCond(std::tuple<Tls..., Tr0>(), std::tuple<Trs...>());
        else
            return concatTrueCond(c, std::tuple<Trs...>());
    }

    template <typename... Ts>
    constexpr auto toIndexSeq(std::tuple<Ts...>) {
        return std::index_sequence<Ts::val...>();
    }

    template <size_t I, size_t... Is>
    constexpr auto removeFromInts() {
        return toIndexSeq(concatTrueCond(std::tuple<>(), std::make_tuple(cond_val<I != Is, Is>()...)));
    }

    template <size_t I, size_t... Is>
    constexpr auto removeFromInts(std::index_sequence<Is...>) {
        return removeFromInts<I, Is...>();
    }

    template <size_t N, size_t... Is>
    constexpr auto removeNthInt(std::index_sequence<Is...> is) {
        return removeFromInts<getNthInt<N>(is), Is...>();
    }
}  // namespace igi