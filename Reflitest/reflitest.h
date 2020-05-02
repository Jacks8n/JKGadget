#pragma once

#include <string_view>
#include <tuple>

#define REFL_META_INFO_NAME meta_info

#define REFL_META_INFO_T_NAME meta_info_t

#define REFL_META_INFO_NULL_ID 0

#define REFL_META_INFO_NULL REFL_META_INFO_NAME<REFL_META_INFO_NULL_ID>

#define REFL_META_INFO_BEGIN_ID REFL_META_INFO_NULL::get_id()

#define REFL_META_INFO_BEGIN REFL_META_INFO_NAME<REFL_META_INFO_BEGIN_ID>

#define REFL_META_INFO_OWNER_T REFL_META_INFO_NULL::owner_t

#define REFL_META_INFO_TYPE(l)    REFL_META_INFO_NAME<l>::get_meta_type()
#define REFL_META_INFO_TYPE_NULL  0
#define REFL_META_INFO_TYPE_ENTRY 1
#define REFL_META_INFO_TYPE_END   2

#define META_BEGIN(type)                                                         \
    template <size_t Id>                                                         \
    struct REFL_META_INFO_NAME {                                                 \
        using owner_t = type;                                                    \
                                                                                 \
        static constexpr size_t get_meta_type() noexcept {                       \
            return REFL_META_INFO_TYPE_NULL;                                     \
        };                                                                       \
                                                                                 \
        static constexpr size_t get_id() noexcept {                              \
            return __LINE__;                                                     \
        }                                                                        \
                                                                                 \
        template <size_t Nth>                                                    \
        static constexpr auto get_nth_meta() noexcept {                          \
            return get_nth_meta_impl<Nth, Id>();                                 \
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
        template <size_t Nth, size_t Lo>                                         \
        static constexpr auto get_nth_meta_impl() noexcept {                     \
            if constexpr (REFL_META_INFO_TYPE(Lo + 1)                            \
                          == REFL_META_INFO_TYPE_ENTRY)                          \
                if constexpr (Nth > 0)                                           \
                    return get_nth_meta_impl<Nth - 1, Lo + 2>();                 \
                else                                                             \
                    return REFL_META_INFO_NAME<Lo + 1>();                        \
            else if constexpr (REFL_META_INFO_TYPE(Lo + 1)                       \
                               == REFL_META_INFO_TYPE_NULL)                      \
                return get_nth_meta_impl<Nth, Lo + 1>();                         \
            else                                                                 \
                return REFL_META_INFO_NULL();                                    \
        }                                                                        \
                                                                                 \
        template <size_t Nth = 0, typename... Ts>                                \
        static constexpr auto get_all_meta_impl(std::tuple<Ts...> t) noexcept {  \
            constexpr auto nthinfo = get_nth_meta<Nth>();                        \
            if constexpr (nthinfo.get_meta_type() == REFL_META_INFO_TYPE_ENTRY)  \
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
                       ? T::get_id()                                             \
                       : get_meta_id_impl(member, std::tuple<Ts...>());          \
        }                                                                        \
                                                                                 \
        template <typename Fn, size_t... Is>                                     \
        static constexpr void foreach_meta_impl(                                 \
            Fn&& fn, std::index_sequence<Is...>) noexcept {                      \
            ((void)fn(get_nth_meta<Is>()), ...);                                 \
        }                                                                        \
    };                                                                           \
    using REFL_META_INFO_T_NAME = REFL_META_INFO_NULL;

#define META(member, ...)                                          \
    template <>                                                    \
    struct REFL_META_INFO_NAME<__LINE__> {                         \
        using owner_t = REFL_META_INFO_NULL::owner_t;              \
                                                                   \
        static constexpr size_t get_meta_type() noexcept {         \
            return REFL_META_INFO_TYPE_ENTRY;                      \
        }                                                          \
                                                                   \
        static constexpr size_t get_id() noexcept {                \
            return __LINE__;                                       \
        }                                                          \
                                                                   \
        static constexpr auto get_attr_all() noexcept {            \
            return std::make_tuple(__VA_ARGS__);                   \
        }                                                          \
                                                                   \
        static constexpr std::string_view member_name() noexcept { \
            return #member;                                        \
        }                                                          \
                                                                   \
        static constexpr auto member_ptr() noexcept {              \
            return &REFL_META_INFO_BEGIN::owner_t::member;         \
        }                                                          \
                                                                   \
        template <size_t Nth>                                      \
        static constexpr auto get_nth_attr() noexcept {            \
            return std::get<Nth>(get_attr_all());                  \
        }                                                          \
                                                                   \
        template <typename T>                                      \
        static constexpr T get_attr() noexcept {                   \
            return std::get<T>(get_attr_all());                    \
        }                                                          \
    };

#define META_END                                           \
    template <>                                            \
    struct REFL_META_INFO_NAME<__LINE__> {                 \
        static constexpr size_t get_meta_type() noexcept { \
            return REFL_META_INFO_TYPE_END;                \
        }                                                  \
    };

#define MetaOf(type) std::decay_t<type>::REFL_META_INFO_T_NAME

#define GetMemberMeta(type, member)                                               \
    ([&]() constexpr {                                                            \
        constexpr size_t __id = type::REFL_META_INFO_T_NAME::get_meta_id(member); \
        static_assert(__id != REFL_META_INFO_NULL_ID, "Member not found");        \
        return type::REFL_META_INFO_NAME<__id>();                                 \
    }())

#define GetMemberType(meta) decltype(__reflitest::reflitest_impl::get_member_type((meta).member_ptr()))

#define GetMemberValue(type, member, instance) ((instance).*GetMemberMeta(type, member).member_ptr())

namespace __reflitest {
    struct reflitest_impl {
        template <class T, class M>
        static M get_member_type(M T::*);
    };
}  // namespace __reflitest
