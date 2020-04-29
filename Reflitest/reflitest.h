#pragma once

#include <string_view>
#include <tuple>

#define REFL_META_INFO_NAME meta_info

#define REFL_META_INFO_T_NAME meta_info_t

#define REFL_META_INFO_NULL REFL_META_INFO_NAME<0>

#define REFL_META_INFO_BEGIN_LINE REFL_META_INFO_NULL::value

#define REFL_META_INFO_BEGIN REFL_META_INFO_NAME<REFL_META_INFO_BEGIN_LINE>

#define REFL_META_INFO_TYPE(l)    REFL_META_INFO_NAME<l>::meta_type
#define REFL_META_INFO_TYPE_NULL  0
#define REFL_META_INFO_TYPE_ENTRY 1
#define REFL_META_INFO_TYPE_END   2

#define META_BEGIN(type)                                                                     \
    template <size_t L>                                                                      \
    struct REFL_META_INFO_NAME {                                                             \
        using owner_t = type;                                                                \
                                                                                             \
        static constexpr size_t meta_type = REFL_META_INFO_TYPE_NULL;                        \
        static constexpr size_t value     = __LINE__;                                        \
                                                                                             \
        template <size_t LMeta, size_t LLo = L, size_t N = 0>                                \
        static constexpr size_t GetOrder() noexcept {                                        \
            if constexpr (LLo + 1 == LMeta)                                                  \
                return N;                                                                    \
            else if constexpr (REFL_META_INFO_TYPE(LLo + 1)                                  \
                               == REFL_META_INFO_TYPE_NULL)                                  \
                return GetOrder<LMeta, LLo + 1, N>();                                        \
            else if constexpr (REFL_META_INFO_TYPE(LLo + 1)                                  \
                               == REFL_META_INFO_TYPE_ENTRY)                                 \
                return GetOrder<LMeta, LLo + 1, N + 1>();                                    \
            else                                                                             \
                return 0;                                                                    \
        }                                                                                    \
                                                                                             \
        template <size_t Nth, size_t LLo = L>                                                \
        static constexpr auto GetMetaInfo() noexcept {                                       \
            if constexpr (REFL_META_INFO_TYPE(LLo + 1)                                       \
                          == REFL_META_INFO_TYPE_ENTRY)                                      \
                if constexpr (Nth > 0)                                                       \
                    return GetMetaInfo<Nth - 1, LLo + 2>();                                  \
                else                                                                         \
                    return REFL_META_INFO_NAME<LLo + 1>();                                   \
            else if constexpr (REFL_META_INFO_TYPE(LLo + 1)                                  \
                               == REFL_META_INFO_TYPE_NULL)                                  \
                return GetMetaInfo<Nth, LLo + 1>();                                          \
            else                                                                             \
                return REFL_META_INFO_NULL();                                                \
        }                                                                                    \
                                                                                             \
        static constexpr auto GetAllMetaInfo() noexcept {                                    \
            return getAllMetaInfo_impl(std::tuple<>());                                      \
        }                                                                                    \
                                                                                             \
        static constexpr auto GetMetaInfo(std::string_view member) noexcept {                \
            return getMetaInfo_impl(member, GetAllMetaInfo());                               \
        }                                                                                    \
                                                                                             \
      private:                                                                               \
        template <size_t Nth = 0, typename... Ts>                                            \
        static constexpr auto getAllMetaInfo_impl(std::tuple<Ts...> t) noexcept {            \
            constexpr auto nthinfo = GetMetaInfo<Nth>();                                     \
            if constexpr (nthinfo.meta_type == REFL_META_INFO_TYPE_ENTRY)                    \
                return getAllMetaInfo_impl<Nth + 1>(std::tuple<Ts..., decltype(nthinfo)>()); \
            else                                                                             \
                return t;                                                                    \
        }                                                                                    \
                                                                                             \
        static constexpr auto getMetaInfo_impl(std::string_view, std::tuple<>) {             \
            return REFL_META_INFO_NAME<0>();                                                 \
        }                                                                                    \
                                                                                             \
        template <typename T, typename... Ts>                                                \
        static constexpr auto getMetaInfo_impl(                                              \
            std::string_view member, std::tuple<T, Ts...> t) {                               \
            return T::member_name() == member                                                \
                       ? T()                                                                 \
                       : getMetaInfo_impl(member, std::tuple<Ts...>());                      \
        }                                                                                    \
    };                                                                                       \
    using REFL_META_INFO_T_NAME = REFL_META_INFO_NAME<REFL_META_INFO_NAME<0>::value>;

#define META(tie, ...)                                                                     \
    template <>                                                                            \
    struct REFL_META_INFO_NAME<__LINE__> {                                                 \
        constexpr REFL_META_INFO_NAME() = default;                                         \
        template <size_t N, typename = std::enable_if_t<(N < __LINE__)>>                   \
        constexpr REFL_META_INFO_NAME(REFL_META_INFO_NAME<N>) { }                          \
                                                                                           \
        static constexpr size_t meta_type = REFL_META_INFO_TYPE_ENTRY;                     \
        static constexpr auto value       = std::make_tuple(__VA_ARGS__);                  \
        static constexpr std::string_view member_name() { return #tie; }                   \
        static constexpr size_t attr_order() {                                             \
            return REFL_META_INFO_BEGIN::GetOrder<__LINE__>();                             \
        }                                                                                  \
        static constexpr auto member_ptr() { return &REFL_META_INFO_BEGIN::owner_t::tie; } \
    };

#define META_GATHER                                                  \
    template <>                                                      \
    struct REFL_META_INFO_NAME<__LINE__> {                           \
        static constexpr size_t meta_type = REFL_META_INFO_TYPE_END; \
    };