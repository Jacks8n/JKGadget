#pragma once

// Notes:
//  0.  Some `requires` constraints duplicate concept codes, which is intended, because these
//      expressions are somehow `cached` and yield identical value in contexts where they are
//      supposed to differ, e.g. declared but not completely defined and after defined
//      Don't know whether it's by design or a bug of clang 10.0 :(
//      These duplication will be marked out

#include "rflite/internal/attribute.h"

#define RFLITE_META_INFO meta_info

#define RFLITE_META_TYPE(type)                                        \
    template <size_t, typename>                                       \
    friend class RFLITE_META_INFO;                                    \
                                                                      \
  public:                                                             \
    static constexpr size_t get_id() noexcept {                       \
        return __LINE__;                                              \
    }                                                                 \
                                                                      \
  private:                                                            \
    static constexpr RFLITE_IMPL meta_type get_meta_type() noexcept { \
        return RFLITE_IMPL meta_type::type;                           \
    }

#define META_B(_type, ...)                                                                                            \
    template <size_t, typename>                                                                                       \
    friend class RFLITE_META_INFO;                                                                                    \
    template <size_t Id, typename Base = RFLITE_IMPL try_get_base_t<_type>>                                           \
    class RFLITE_META_INFO {                                                                                          \
        RFLITE_META_TYPE(null)                                                                                        \
                                                                                                                      \
      public:                                                                                                         \
        using class_t = _type;                                                                                        \
        using base_t  = Base;                                                                                         \
    };                                                                                                                \
    template <typename Base>                                                                                          \
    class RFLITE_META_INFO<__LINE__, Base> {                                                                          \
        RFLITE_META_TYPE(null)                                                                                        \
                                                                                                                      \
      public:                                                                                                         \
        using class_t = _type;                                                                                        \
                                                                                                                      \
      private:                                                                                                        \
        template <size_t Nth, size_t Lo = __LINE__>                                                                   \
        static constexpr auto get_nth_meta_impl() noexcept {                                                          \
            constexpr RFLITE_IMPL meta_type type = RFLITE_META_INFO<Lo>::get_meta_type();                             \
            if constexpr (type == RFLITE_IMPL meta_type::entry)                                                       \
                if constexpr (Nth > 0)                                                                                \
                    return get_nth_meta_impl<Nth - 1, Lo + 1>();                                                      \
                else                                                                                                  \
                    return class_t::RFLITE_META_INFO<Lo>();                                                           \
            else if constexpr (type == RFLITE_IMPL meta_type::null)                                                   \
                return get_nth_meta_impl<Nth, Lo + 1>();                                                              \
            else                                                                                                      \
                return RFLITE_META_INFO();                                                                            \
        }                                                                                                             \
                                                                                                                      \
        template <size_t Lo = __LINE__, typename... Ts>                                                               \
        static constexpr auto get_meta_all_impl(::std::tuple<Ts...> t) noexcept {                                     \
            constexpr auto nthinfo = get_nth_meta_impl<0, Lo>();                                                      \
            if constexpr (nthinfo.get_meta_type() == RFLITE_IMPL meta_type::entry)                                    \
                return get_meta_all_impl<nthinfo.get_id() + 1>(::std::tuple<Ts..., decltype(nthinfo)>());             \
            else                                                                                                      \
                return t;                                                                                             \
        }                                                                                                             \
                                                                                                                      \
        static constexpr size_t get_meta_id_impl(::std::string_view member, ::std::tuple<>) noexcept {                \
            return RFLITE_IMPL meta_null_id;                                                                          \
        }                                                                                                             \
                                                                                                                      \
        template <typename T, typename... Ts>                                                                         \
        static constexpr size_t get_meta_id_impl(::std::string_view member, ::std::tuple<T, Ts...>) noexcept {        \
            return T::member_name() == member ? T::get_id() : get_meta_id_impl(member, ::std::tuple<Ts...>());        \
        }                                                                                                             \
                                                                                                                      \
        template <RFLITE member_type Member, typename Fn>                                                             \
        static constexpr ::std::tuple<> foreach_impl(Fn &&fn, ::std::tuple<>) noexcept {                              \
            return ::std::tuple<>();                                                                                  \
        }                                                                                                             \
                                                                                                                      \
        template <RFLITE member_type Member, typename Fn, typename TMeta, typename... TMetas>                         \
        static constexpr auto foreach_impl(Fn &&fn, ::std::tuple<TMeta, TMetas...>) {                                 \
            if constexpr (RFLITE has_flag(Member, RFLITE_IMPL member_ptr_type_v<decltype(TMeta::member_ptr())>)) {    \
                if constexpr (::std::is_same_v<void, decltype(fn(TMeta()))>)                                          \
                    fn(TMeta());                                                                                      \
                else                                                                                                  \
                    return ::std::tuple_cat(fn(TMeta()),                                                              \
                                            foreach_impl<Member>(::std::forward<Fn>(fn), ::std::tuple<TMetas...>())); \
            }                                                                                                         \
            return foreach_impl<Member>(::std::forward<Fn>(fn), ::std::tuple<TMetas...>());                           \
        }                                                                                                             \
                                                                                                                      \
      public:                                                                                                         \
        using base_t = Base;                                                                                          \
                                                                                                                      \
        static constexpr auto attributes = RFLITE_IMPL make_attributes<class_t>(__VA_ARGS__);                         \
                                                                                                                      \
        static constexpr ::std::string_view name() noexcept {                                                         \
            return #_type;                                                                                            \
        }                                                                                                             \
                                                                                                                      \
        template <size_t Nth>                                                                                         \
        static constexpr auto get_nth_meta() noexcept {                                                               \
            return get_nth_meta_impl<Nth>();                                                                          \
        }                                                                                                             \
                                                                                                                      \
        static constexpr auto get_meta_all() noexcept {                                                               \
            return get_meta_all_impl(::std::tuple<>());                                                               \
        }                                                                                                             \
                                                                                                                      \
        static constexpr size_t get_meta_id(::std::string_view member) noexcept {                                     \
            return get_meta_id_impl(member, get_meta_all());                                                          \
        }                                                                                                             \
                                                                                                                      \
        static constexpr size_t get_meta_count() noexcept {                                                           \
            return ::std::tuple_size_v<decltype(get_meta_all())>;                                                     \
        }                                                                                                             \
                                                                                                                      \
        template <RFLITE member_type Member = RFLITE member_type::field, typename Fn>                                 \
        static constexpr decltype(auto) foreach(Fn &&fn) {                                                            \
            return foreach_impl<Member>(::std::forward<Fn>(fn), get_meta_all());                                      \
        }                                                                                                             \
                                                                                                                      \
        template <RFLITE member_type Member = RFLITE member_type::field, typename TCallback>                          \
        static constexpr decltype(auto) find_meta(::std::string_view member, TCallback &&callback) {                  \
            return foreach<Member>([&](auto &&meta) {                                                                 \
                if (meta.member_name() == member)                                                                     \
                    callback(meta);                                                                                   \
            });                                                                                                       \
        }                                                                                                             \
    };

#define META(member, ...)                                                                       \
    template <typename _>                                                                       \
    class RFLITE_META_INFO<__LINE__, _> {                                                       \
        RFLITE_META_TYPE(entry)                                                                 \
                                                                                                \
      public:                                                                                   \
        using class_t = typename RFLITE_META_INFO<RFLITE_IMPL meta_null_id>::class_t;           \
                                                                                                \
        static constexpr auto attributes = RFLITE_IMPL make_attributes<class_t>(__VA_ARGS__);   \
                                                                                                \
        static constexpr ::std::string_view member_name() noexcept {                            \
            return #member;                                                                     \
        }                                                                                       \
                                                                                                \
        static constexpr auto member_ptr() noexcept {                                           \
            return &class_t::member;                                                            \
        }                                                                                       \
                                                                                                \
        template <typename T>                                                                   \
        static constexpr bool is_type() noexcept {                                              \
            return ::std::is_same_v<T, RFLITE_IMPL member_ptr_value_t<decltype(member_ptr())>>; \
        }                                                                                       \
                                                                                                \
        static constexpr bool is_function() noexcept {                                          \
            return ::std::is_member_function_pointer_v<decltype(member_ptr())>;                 \
        }                                                                                       \
                                                                                                \
        template <template <typename> typename T>                                               \
        static constexpr bool satisfy_traits() noexcept {                                       \
            using member_t = RFLITE_IMPL member_ptr_value_t<decltype(member_ptr())>;            \
            return T<member_t>::value;                                                          \
        }                                                                                       \
                                                                                                \
      private:                                                                                  \
        static constexpr auto get_member_traits() {                                             \
            return RFLITE_IMPL member_traits<decltype(member_ptr())>();                         \
        }                                                                                       \
                                                                                                \
      public:                                                                                   \
        template <typename... T>                                                                \
        static constexpr decltype(auto) map(T &&... t) noexcept {                               \
            return get_member_traits().map(::std::forward<T>(t)..., member_ptr());              \
        }                                                                                       \
                                                                                                \
        template <typename... Ts>                                                               \
        static constexpr decltype(auto) invoke(Ts &&... ts) {                                   \
            return get_member_traits().invoke(member_ptr(), ::std::forward<Ts>(ts)...);         \
        }                                                                                       \
    };

#define META_E                                \
    template <typename _>                     \
    class RFLITE_META_INFO<__LINE__ + 1, _> { \
        RFLITE_META_TYPE(end)                 \
    };

#define META_EMPTY(type, ...) \
    META_B(type, __VA_ARGS__) \
    META_E

RFLITE_IMPL_NS {
    static constexpr size_t meta_null_id = 0;

    template <template <size_t, typename> typename>
    class has_meta_impl { };

    template <typename T>
    concept has_meta = requires { typename has_meta_impl<T::template RFLITE_META_INFO>; };

    template <typename T>
    struct try_get_base {
        using type = T;
    };

    template <typename T>
    // Duplication of `RFLITE has_meta`
    requires requires { typename has_meta_impl<T::template RFLITE_META_INFO>; }
    struct try_get_base<T> {
        using type = typename T::template RFLITE_META_INFO<0>::base_t;
    };

    template <typename T>
    using try_get_base_t = typename try_get_base<T>::type;

    template <typename T>
    struct null_meta { };

    template <typename T>
    struct null_meta_traits {
        static constexpr bool value = false;

        using type = T;
    };

    template <typename T>
    struct null_meta_traits<null_meta<T>> {
        static constexpr bool value = true;

        using type = T;
    };

    template <typename T>
    struct meta_of_impl {
        using type = null_meta<T>;
    };

    template <has_meta T>
    struct meta_of_impl<T> {
        using type = typename T::template RFLITE_META_INFO<T::template RFLITE_META_INFO<RFLITE_IMPL meta_null_id>::get_id()>;
    };

    template <typename... Ts>
    struct meta_of_foreach;
}

RFLITE_NS {
    template <typename T>
    static constexpr bool is_null_meta_v = RFLITE_IMPL null_meta_traits<T>::value;

    template <typename T>
    using remove_null_meta_t = typename RFLITE_IMPL null_meta_traits<T>::type;

    template <typename T>
    using meta_of = typename RFLITE_IMPL meta_of_impl<T>::type;

    template <typename T>
    using meta_d_of = meta_of<::std::decay_t<T>>;

    template <typename T>
    using base_of = typename meta_of<T>::base_t;

    template <typename T>
    static constexpr bool is_base_v = ::std::is_same_v<T, base_of<T>>;

    template <typename... Ts>
    struct RFLITE_IMPL meta_of_foreach {
        template <typename Fn>
        static constexpr auto invoke(Fn &&fn) {
            return ::std::make_tuple((fn(meta_of<Ts>()), ...));
        }
    };

    template <typename... Ts>
    struct RFLITE_IMPL meta_of_foreach<::std::tuple<Ts...>> {
        template <typename Fn>
        static constexpr auto invoke(Fn &&fn) {
            return meta_of_foreach<Ts...>::invoke(::std::forward<Fn>(fn));
        }
    };

    template <typename... Ts, typename Fn>
    constexpr auto foreach_meta_of(Fn && fn) {
        return RFLITE_IMPL meta_of_foreach<Ts...>(::std::forward<Fn>(fn));
    }
}

#pragma region helper macro

#define GetMemberMeta(type, member)                                        \
    ([&]() constexpr {                                                     \
        constexpr size_t id = RFLITE meta_of<type>::get_meta_id(member);   \
        static_assert(id != RFLITE_IMPL meta_null_id, "Member not found"); \
        return type::RFLITE_META_INFO<id>();                               \
    }())

#pragma endregion
