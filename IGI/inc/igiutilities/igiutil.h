#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

namespace igi {
    template <typename T0, typename... Ts>
    constexpr bool all_same_v = (std::is_same_v<T0, Ts> &&...);

    template <typename TTo, typename... TFroms>
    constexpr bool all_convertible_v = (std::is_convertible_v<TFroms, TTo> && ...);

    template <size_t I, size_t... Is>
    constexpr size_t GetFirstInt() {
        return I;
    }

    template <size_t N, size_t... Is>
    constexpr size_t GetNthInt() {
        return std::get<N>(std::make_tuple(Is...));
    }

    template <bool B, auto I>
    struct cond_val {
        static constexpr bool cond  = B;
        static constexpr auto value = I;
    };

    template <typename... Ts>
    constexpr std::tuple<Ts...> ConcatTrueCond(std::tuple<Ts...> c, std::tuple<>) {
        return c;
    }

    template <typename... Tls, typename Tr0, typename... Trs>
    constexpr auto ConcatTrueCond(std::tuple<Tls...> c, std::tuple<Tr0, Trs...>) {
        if constexpr (Tr0::cond)
            return ConcatTrueCond(std::tuple<Tls..., Tr0>(), std::tuple<Trs...>());
        else
            return ConcatTrueCond(c, std::tuple<Trs...>());
    }

    template <typename... Ts>
    constexpr auto ToIndexSeq(std::tuple<Ts...>) {
        return std::index_sequence<Ts::value...>();
    }

    template <typename T, size_t... Is>
    constexpr auto MakeTupleN(std::index_sequence<Is...>) {
        return std::tuple<std::conditional_t<false, decltype(Is), T>...>();
    }

    template <typename T, size_t N>
    constexpr auto MakeTupleN() {
        return MakeTupleN<T>(std::make_index_sequence<N>());
    }

    template <size_t I, size_t... Is>
    constexpr auto RemoveFromInts() {
        return ToIndexSeq(ConcatTrueCond(std::tuple<>(), std::make_tuple(cond_val<I != Is, Is>()...)));
    }

    template <size_t I, size_t... Is>
    constexpr auto RemoveFromInts(std::index_sequence<Is...>) {
        return RemoveFromInts<I, Is...>();
    }

    template <typename TFn, size_t... Is, typename... Ts,
              std::enable_if_t<std::is_invocable_v<TFn, Ts...>, int> = 0>
    constexpr void ForCE(TFn &&fn, std::index_sequence<Is...>, Ts &&... ts) {
        (((void)Is, (void)fn(std::forward<Ts>(ts)...)), ...);
    }

    template <typename TFn, size_t... Is, typename... Ts,
              std::enable_if_t<std::is_invocable_v<TFn, size_t, Ts...>, int> = 0>
    constexpr auto ForCE(TFn &&fn, std::index_sequence<Is...>, Ts &&... ts) {
        return std::make_tuple(fn(Is, std::forward<Ts>(ts)...)...);
    }

    template <size_t N, typename TFn, typename... Ts>
    constexpr auto ForCE(TFn &&fn, Ts &&... ts) {
        return ForCE(std::forward<TFn>(fn), std::make_index_sequence<N>(), std::forward<Ts>(ts)...);
    }
}  // namespace igi