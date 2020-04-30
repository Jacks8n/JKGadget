#pragma once

#include <string_view>
#include <tuple>

#define REFL_META_INFO_NAME meta_info

#define REFL_META_INFO_T_NAME meta_info_t

#define REFL_META_INFO_NULL_ID 0

#define REFL_META_INFO_NULL REFL_META_INFO_NAME<REFL_META_INFO_NULL_ID>

#define REFL_META_INFO_BEGIN_LINE REFL_META_INFO_NULL::id

#define REFL_META_INFO_BEGIN REFL_META_INFO_NAME<REFL_META_INFO_BEGIN_LINE>

#define REFL_META_INFO_OWNER_T REFL_META_INFO_NULL::owner_t

#define REFL_META_INFO_TYPE(l)    REFL_META_INFO_NAME<l>::meta_t
#define REFL_META_INFO_TYPE_NULL  0
#define REFL_META_INFO_TYPE_ENTRY 1
#define REFL_META_INFO_TYPE_END   2

#define META_BEGIN(type)                                                         \
    template <size_t L>                                                          \
    struct REFL_META_INFO_NAME {                                                 \
        using owner_t = type;                                                    \
                                                                                 \
        static constexpr size_t meta_t = REFL_META_INFO_TYPE_NULL;               \
        static constexpr size_t id     = __LINE__;                               \
                                                                                 \
        template <size_t LMeta, size_t LLo = L, size_t N = 0>                    \
        static constexpr size_t get_order() noexcept {                           \
            if constexpr (LLo + 1 == LMeta)                                      \
                return N;                                                        \
            else if constexpr (REFL_META_INFO_TYPE(LLo + 1)                      \
                               == REFL_META_INFO_TYPE_NULL)                      \
                return get_order<LMeta, LLo + 1, N>();                           \
            else if constexpr (REFL_META_INFO_TYPE(LLo + 1)                      \
                               == REFL_META_INFO_TYPE_ENTRY)                     \
                return get_order<LMeta, LLo + 1, N + 1>();                       \
            else                                                                 \
                return REFL_META_INFO_NULL_ID;                                   \
        }                                                                        \
                                                                                 \
        template <size_t Nth, size_t LLo = L>                                    \
        static constexpr auto get_nth_meta() noexcept {                          \
            if constexpr (REFL_META_INFO_TYPE(LLo + 1)                           \
                          == REFL_META_INFO_TYPE_ENTRY)                          \
                if constexpr (Nth > 0)                                           \
                    return get_nth_meta<Nth - 1, LLo + 2>();                     \
                else                                                             \
                    return REFL_META_INFO_NAME<LLo + 1>();                       \
            else if constexpr (REFL_META_INFO_TYPE(LLo + 1)                      \
                               == REFL_META_INFO_TYPE_NULL)                      \
                return get_nth_meta<Nth, LLo + 1>();                             \
            else                                                                 \
                return REFL_META_INFO_NULL();                                    \
        }                                                                        \
                                                                                 \
        static constexpr auto get_all_meta() noexcept {                          \
            return get_all_meta_impl(std::tuple<>());                            \
        }                                                                        \
                                                                                 \
        static constexpr size_t get_meta_id(std::string_view member) noexcept {  \
            return get_meta_id_impl(member, get_all_meta());                     \
        }                                                                        \
                                                                                 \
        static constexpr size_t get_meta_count() noexcept {                      \
            return std::tuple_size_v<decltype(get_all_meta())>;                  \
        }                                                                        \
                                                                                 \
        template <typename Fn>                                                   \
        static constexpr void foreach_meta(Fn&& fn) noexcept {                   \
            foreach_meta_impl(fn, std::make_index_sequence<get_meta_count()>()); \
        }                                                                        \
                                                                                 \
      private:                                                                   \
        template <size_t Nth = 0, typename... Ts>                                \
        static constexpr auto get_all_meta_impl(std::tuple<Ts...> t) noexcept {  \
            constexpr auto nthinfo = get_nth_meta<Nth>();                        \
            if constexpr (nthinfo.meta_t == REFL_META_INFO_TYPE_ENTRY)           \
                return get_all_meta_impl<Nth + 1>(                               \
                    std::tuple<Ts..., decltype(nthinfo)>());                     \
            else                                                                 \
                return t;                                                        \
        }                                                                        \
                                                                                 \
        static constexpr size_t get_meta_id_impl(                                \
            std::string_view, std::tuple<>) noexcept {                           \
            return REFL_META_INFO_NULL_ID;                                       \
        }                                                                        \
                                                                                 \
        template <typename T, typename... Ts>                                    \
        static constexpr size_t get_meta_id_impl(                                \
            std::string_view member, std::tuple<T, Ts...> t) noexcept {          \
            return T::member_name() == member                                    \
                       ? T::id                                                   \
                       : get_meta_id_impl(member, std::tuple<Ts...>());          \
        }                                                                        \
                                                                                 \
        template <typename Fn, size_t... Is>                                     \
        static constexpr void foreach_meta_impl(                                 \
            Fn&& fn, std::index_sequence<Is...>) noexcept {                      \
            ((void)fn(get_nth_meta<Is>()), ...);                                 \
        }                                                                        \
    };                                                                           \
    using REFL_META_INFO_T_NAME = REFL_META_INFO_BEGIN;

#define META(member, ...)                                                            \
    template <>                                                                      \
    struct REFL_META_INFO_NAME<__LINE__> {                                           \
        static constexpr size_t meta_t = REFL_META_INFO_TYPE_ENTRY;                  \
        static constexpr size_t id     = __LINE__;                                   \
                                                                                     \
        using owner_t                    = REFL_META_INFO_NULL::owner_t;             \
        static constexpr auto attributes = std::make_tuple(__VA_ARGS__);             \
                                                                                     \
        static constexpr std::string_view member_name() noexcept { return #member; } \
                                                                                     \
        static constexpr size_t attr_order() noexcept {                              \
            return REFL_META_INFO_BEGIN::get_order<__LINE__>();                      \
        }                                                                            \
                                                                                     \
        static constexpr auto member_ptr() noexcept {                                \
            return &REFL_META_INFO_BEGIN::owner_t::member;                           \
        }                                                                            \
                                                                                     \
        template <size_t Nth>                                                        \
        static constexpr auto get_nth_attr() noexcept {                              \
            return std::get<Nth>(attributes);                                        \
        }                                                                            \
    };

#define META_END                                                  \
    template <>                                                   \
    struct REFL_META_INFO_NAME<__LINE__> {                        \
        static constexpr size_t meta_t = REFL_META_INFO_TYPE_END; \
    };

#define GetMemberMeta(type, member)                                               \
    ([&]() constexpr {                                                            \
        constexpr size_t __id = type::REFL_META_INFO_T_NAME::get_meta_id(member); \
        static_assert(__id != REFL_META_INFO_NULL_ID, "Member not found");        \
        return type::REFL_META_INFO_NAME<__id>();                                 \
    }())

struct reflitest_impl {
    template <class T, class M>
    static M get_member_type(M T::*);
};

#define GetMemberType(meta) decltype(reflitest_impl::get_member_type((meta).member_ptr()))

#define GetMemberValue(type, member, instance) ((instance).*GetMemberMeta(type, member).member_ptr())