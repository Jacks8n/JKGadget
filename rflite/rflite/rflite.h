#pragma once

#pragma region static reflection

#include <assert.h>
#include <concepts>
#include <string_view>
#include <tuple>

#define RFLITE      ::rflite::
#define RFLITE_IMPL ::rflite::impl::

#define RFLITE_META_INFO_NAME meta_info

#define RFLITE_META_INFO_NULL_ID 0

#define RFLITE_META_INFO_NULL RFLITE_META_INFO_NAME<RFLITE_META_INFO_NULL_ID>

#define RFLITE_META_INFO_BEGIN_ID RFLITE_META_INFO_NULL::get_id()

#define RFLITE_META_INFO_BEGIN RFLITE_META_INFO_NAME<RFLITE_META_INFO_BEGIN_ID>

#define RFLITE_META_INFO_TYPE_OF(l) RFLITE_META_INFO_NAME<l>::get_meta_type()
#define RFLITE_META_INFO_TYPE_NULL  0
#define RFLITE_META_INFO_TYPE_ENTRY 1
#define RFLITE_META_INFO_TYPE_END   2

#define META_B(_type, ...)                                                                                                                                             \
    template <size_t>                                                                                                                                                  \
    friend struct RFLITE_META_INFO_NAME;                                                                                                                               \
    template <size_t Id, typename Base = RFLITE_IMPL base_of_t<_type>>                                                                                                 \
    class RFLITE_META_INFO_NAME {                                                                                                                                      \
        using attribute_tuple_t = decltype(::std::make_tuple(__VA_ARGS__));                                                                                            \
                                                                                                                                                                       \
        static constexpr const attribute_tuple_t _attributes = ::std::make_tuple(__VA_ARGS__);                                                                         \
                                                                                                                                                                       \
        template <size_t Nth, size_t Lo = Id>                                                                                                                          \
        static constexpr auto get_nth_meta_impl() noexcept {                                                                                                           \
            if constexpr (RFLITE_META_INFO_TYPE_OF(Lo) == RFLITE_META_INFO_TYPE_ENTRY)                                                                                 \
                if constexpr (Nth > 0)                                                                                                                                 \
                    return get_nth_meta_impl<Nth - 1, Lo + 1>();                                                                                                       \
                else                                                                                                                                                   \
                    return RFLITE_META_INFO_NAME<Lo>();                                                                                                                \
            else if constexpr (RFLITE_META_INFO_TYPE_OF(Lo) == RFLITE_META_INFO_TYPE_NULL)                                                                             \
                return get_nth_meta_impl<Nth, Lo + 1>();                                                                                                               \
            else                                                                                                                                                       \
                return RFLITE_META_INFO_NAME();                                                                                                                        \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <size_t Lo = Id, typename... Ts>                                                                                                                      \
        static constexpr auto get_meta_all_impl(::std::tuple<Ts...> t) noexcept {                                                                                      \
            constexpr auto nthinfo = get_nth_meta_impl<0, Lo>();                                                                                                       \
            if constexpr (nthinfo.get_meta_type() == RFLITE_META_INFO_TYPE_ENTRY)                                                                                      \
                return get_meta_all_impl<nthinfo.get_id() + 1>(::std::tuple<Ts..., decltype(nthinfo)>());                                                              \
            else                                                                                                                                                       \
                return t;                                                                                                                                              \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        static constexpr size_t get_meta_id_impl(const ::std::string_view &member, ::std::tuple<>) noexcept {                                                          \
            return RFLITE_META_INFO_NULL_ID;                                                                                                                           \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename T, typename... Ts>                                                                                                                          \
        static constexpr size_t get_meta_id_impl(const ::std::string_view &member, ::std::tuple<T, Ts...>) noexcept {                                                  \
            return T::member_name() == member ? T::get_id() : get_meta_id_impl(member, ::std::tuple<Ts...>());                                                         \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename Fn, size_t... Is>                                                                                                                           \
        static constexpr void foreach_impl(Fn &&fn, ::std::integer_sequence<bool>, ::std::index_sequence<Is...>, ::std::index_sequence<>) noexcept {                   \
            ((void)fn(get_nth_meta_impl<Is>()), ...);                                                                                                                  \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename Fn, size_t... Is>                                                                                                                           \
        static constexpr auto foreach_impl(Fn &&fn, ::std::integer_sequence<bool>, ::std::index_sequence<Is...>, ::std::index_sequence<>) noexcept                     \
            requires(!::std::is_same_v<void, decltype(fn(get_nth_meta_impl<Is>()...))>) {                                                                              \
            return ::std::make_tuple((fn(get_nth_meta_impl<Is>()), ...));                                                                                              \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename Fn, bool B, bool... Bs, size_t I, size_t... IL, size_t... IR>                                                                               \
        static constexpr auto foreach_impl(Fn &&fn, ::std::integer_sequence<bool, B, Bs...>, ::std::index_sequence<IL...>, ::std::index_sequence<I, IR...>) noexcept { \
            constexpr ::std::integer_sequence<bool, Bs...> bseq;                                                                                                       \
            constexpr ::std::index_sequence<IR...> iseq;                                                                                                               \
            if constexpr (B)                                                                                                                                           \
                return foreach_impl(::std::forward<Fn>(fn), bseq, ::std::index_sequence<IL..., I>(), iseq);                                                            \
            else                                                                                                                                                       \
                return foreach_impl(::std::forward<Fn>(fn), bseq, ::std::index_sequence<IL...>(), iseq);                                                               \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <RFLITE member_type M, typename Fn, size_t... Is>                                                                                                     \
        static constexpr auto foreach_impl(Fn &&fn, ::std::index_sequence<Is...> is) noexcept {                                                                        \
            constexpr auto seq = ::std::integer_sequence<bool, RFLITE has_flag(RFLITE member_ptr_type_v<decltype(get_nth_meta<Is>().member_ptr())>, M)...>();          \
            return foreach_impl(::std::forward<Fn>(fn), seq, ::std::index_sequence<>(), is);                                                                           \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <template <typename...> typename T, typename TTuple, size_t... Is>                                                                                    \
        static constexpr auto has_attr_impl(::std::index_sequence<Is...>) noexcept {                                                                                   \
            return (RFLITE is_specialization_of<T, ::std::tuple_element_t<Is, TTuple>> || ...);                                                                        \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <template <typename...> typename T, typename... Ts>                                                                                                   \
        static constexpr auto get_attr_impl(::std::tuple<Ts...> &&t) noexcept {                                                                                        \
            return ::std::get<RFLITE index_of_first_v<true, (RFLITE is_specialization_of<T, ::std::remove_reference_t<Ts>>)...>>(t);                                   \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename T, typename... Ts>                                                                                                                          \
        static constexpr T get_attr_impl(::std::tuple<Ts...> &&t) noexcept {                                                                                           \
            constexpr size_t index = RFLITE index_of_first_t_v<T, ::std::remove_reference_t<Ts>...>;                                                                   \
            if constexpr (index < sizeof...(Ts))                                                                                                                       \
                return ::std::get<index>(t);                                                                                                                           \
            else                                                                                                                                                       \
                return T();                                                                                                                                            \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename T>                                                                                                                                          \
        static constexpr decltype(auto) get_typed_attr(T &&t) noexcept {                                                                                               \
            if constexpr (RFLITE is_attribute_v<T>)                                                                                                                    \
                return t.template get<_type>();                                                                                                                        \
            else                                                                                                                                                       \
                return ::std::forward<T>(t);                                                                                                                           \
        }                                                                                                                                                              \
                                                                                                                                                                       \
      public:                                                                                                                                                          \
        using owner_t = _type;                                                                                                                                         \
        using base_t  = Base;                                                                                                                                          \
                                                                                                                                                                       \
        static constexpr size_t get_meta_type() noexcept {                                                                                                             \
            return RFLITE_META_INFO_TYPE_NULL;                                                                                                                         \
        };                                                                                                                                                             \
                                                                                                                                                                       \
        static constexpr size_t get_id() noexcept {                                                                                                                    \
            return __LINE__;                                                                                                                                           \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        static constexpr ::std::string_view name() noexcept {                                                                                                          \
            return #_type;                                                                                                                                             \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <size_t Nth>                                                                                                                                          \
        static constexpr auto get_nth_meta() noexcept {                                                                                                                \
            return get_nth_meta_impl<Nth>();                                                                                                                           \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        static constexpr auto get_meta_all() noexcept {                                                                                                                \
            return get_meta_all_impl(::std::tuple<>());                                                                                                                \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        static constexpr size_t get_meta_id(const ::std::string_view &member) noexcept {                                                                               \
            return get_meta_id_impl(member, get_meta_all());                                                                                                           \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        static constexpr size_t get_meta_count() noexcept {                                                                                                            \
            return ::std::tuple_size_v<decltype(get_meta_all())>;                                                                                                      \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <RFLITE member_type M = RFLITE member_type::field, typename Fn>                                                                                       \
        static constexpr auto foreach(Fn &&fn) noexcept {                                                                                                              \
            return foreach_impl<M>(::std::forward<Fn>(fn), ::std::make_index_sequence<get_meta_count()>());                                                            \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <RFLITE member_type M = RFLITE member_type::field, typename Fn>                                                                                       \
        static constexpr void find_meta(::std::string_view member, Fn &&fn) {                                                                                          \
            foreach<M>([&](auto &&meta) { meta.member_name() == member && (fn(meta), true); });                                                                        \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        static constexpr const auto &get_attr_all() noexcept {                                                                                                         \
            return _attributes;                                                                                                                                        \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        static constexpr auto get_attr_count() noexcept {                                                                                                              \
            return ::std::tuple_size_v<attribute_tuple_t>;                                                                                                             \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename T>                                                                                                                                          \
        static constexpr bool has_attr() noexcept {                                                                                                                    \
            return RFLITE tuple_has_v<T, attribute_tuple_t>;                                                                                                           \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <template <typename...> typename T>                                                                                                                   \
        static constexpr bool has_attr() noexcept {                                                                                                                    \
            return has_attr_impl<T, attribute_tuple_t>(::std::make_index_sequence<get_attr_count()>());                                                                \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <size_t Nth>                                                                                                                                          \
        static constexpr auto get_nth_attr() noexcept {                                                                                                                \
            return ::std::get<Nth>(get_attr_all());                                                                                                                    \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <typename T>                                                                                                                                          \
        static constexpr T get_attr() noexcept {                                                                                                                       \
            return get_typed_attr(::std::get<T>(get_attr_all()));                                                                                                      \
        }                                                                                                                                                              \
                                                                                                                                                                       \
        template <template <typename...> typename T>                                                                                                                   \
        static constexpr auto get_attr() noexcept {                                                                                                                    \
            return get_typed_attr(get_attr_impl(get_attr_impl<T>(get_attr_all(), ::std::make_index_sequence<get_attr_count()>())));                                    \
        }                                                                                                                                                              \
    };

#define META(member, ...)                                                                                            \
    template <typename _>                                                                                            \
    class RFLITE_META_INFO_NAME<__LINE__, _> {                                                                       \
        using attribute_tuple_t = decltype(::std::make_tuple(__VA_ARGS__));                                          \
                                                                                                                     \
        static constexpr const attribute_tuple_t _attributes = ::std::make_tuple(__VA_ARGS__);                       \
                                                                                                                     \
      public:                                                                                                        \
        using owner_t = typename RFLITE_META_INFO_NULL::owner_t;                                                     \
                                                                                                                     \
        static constexpr size_t get_meta_type() noexcept {                                                           \
            return RFLITE_META_INFO_TYPE_ENTRY;                                                                      \
        }                                                                                                            \
                                                                                                                     \
        static constexpr size_t get_id() noexcept {                                                                  \
            return __LINE__;                                                                                         \
        }                                                                                                            \
                                                                                                                     \
        static constexpr ::std::string_view member_name() noexcept {                                                 \
            return #member;                                                                                          \
        }                                                                                                            \
                                                                                                                     \
        static constexpr decltype(auto) member_ptr() noexcept {                                                      \
            return &owner_t::member;                                                                                 \
        }                                                                                                            \
                                                                                                                     \
        static constexpr const auto &get_attr_all() noexcept {                                                       \
            return _attributes;                                                                                      \
        }                                                                                                            \
                                                                                                                     \
        template <size_t Nth>                                                                                        \
        static constexpr auto get_nth_attr() noexcept {                                                              \
            return ::std::get<Nth>(get_attr_all());                                                                  \
        }                                                                                                            \
                                                                                                                     \
        template <typename T>                                                                                        \
        static constexpr bool has_attr() noexcept {                                                                  \
            return RFLITE tuple_has_v<T, attribute_tuple_t>;                                                         \
        }                                                                                                            \
                                                                                                                     \
        template <typename T>                                                                                        \
        static constexpr T get_attr() noexcept {                                                                     \
            return ::std::get<T>(get_attr_all());                                                                    \
        }                                                                                                            \
                                                                                                                     \
        template <typename T>                                                                                        \
        static constexpr bool is_type() noexcept {                                                                   \
            return ::std::is_same_v<T, RFLITE_IMPL member_ptr_value_t<decltype(member_ptr())>>;                      \
        }                                                                                                            \
                                                                                                                     \
        static constexpr bool is_function() noexcept {                                                               \
            return ::std::is_member_function_pointer_v<decltype(member_ptr())>;                                      \
        }                                                                                                            \
                                                                                                                     \
        template <template <typename> typename T>                                                                    \
        static constexpr bool satisfy_traits() noexcept {                                                            \
            using member_t = RFLITE_IMPL member_ptr_value_t<decltype(member_ptr())>;                                 \
            return T<member_t>::value;                                                                               \
        }                                                                                                            \
                                                                                                                     \
        template <typename... T>                                                                                     \
        static constexpr decltype(auto) map(T &&... t) noexcept {                                                    \
            return RFLITE_IMPL rflite_impl<decltype(member_ptr())>::map(::std::forward<T>(t)..., member_ptr());      \
        }                                                                                                            \
                                                                                                                     \
        template <typename... Ts>                                                                                    \
        static constexpr decltype(auto) invoke(Ts &&... ts) {                                                        \
            return RFLITE_IMPL rflite_impl<decltype(member_ptr())>::invoke(member_ptr(), ::std::forward<Ts>(ts)...); \
        }                                                                                                            \
    };

#define META_E                                             \
    template <typename _>                                  \
    class RFLITE_META_INFO_NAME<__LINE__ + 1, _> {         \
      public:                                              \
        static constexpr size_t get_meta_type() noexcept { \
            return RFLITE_META_INFO_TYPE_END;              \
        }                                                  \
    };

#define META_EMPTY(type, ...) \
    META_B(type, __VA_ARGS__) \
    META_E

namespace rflite {
    enum class member_type : size_t {
        null            = 0,
        field           = 1,
        field_static    = 2,
        function        = 4,
        function_const  = 8,
        function_static = 16,
        function_this   = function | function_const,
        any_field       = field | field_static,
        any_function    = function_this | function_static,
        any_static      = field_static | function_static,
        any             = any_field | any_function
    };

    constexpr bool has_flag(member_type type, member_type flag) {
        return static_cast<size_t>(type) & static_cast<size_t>(flag);
    }

    constexpr bool is_field(member_type type) {
        return has_flag(type, member_type::any_field);
    }

    constexpr bool is_function(member_type type) {
        return has_flag(type, member_type::any_function);
    }

    constexpr bool is_static(member_type type) {
        return has_flag(type, member_type::any_static);
    }

    namespace impl {
        template <typename T>
        struct rflite_impl;

        template <typename T>
        struct rflite_impl<T *> {
            using value_t = T;

            static constexpr member_type type = member_type::field_static;

            explicit constexpr rflite_impl(T *) { }

            static constexpr decltype(auto) map(T *ptr) noexcept {
                return *ptr;
            }

            template <typename... _>
            static constexpr int map(_ &&...) noexcept {
                static_assert(sizeof...(_) != sizeof...(_), "bad mapping of static variables");
                return 0;
            }

            template <typename... _>
            static constexpr int invoke(_ &&...) noexcept {
                static_assert(sizeof...(_) != sizeof...(_), "variables can't be invoked");
                return 0;
            }
        };

        template <typename T, typename C>
        struct rflite_impl<T C::*> {
            using value_t = T;
            using class_t = C;

            static constexpr member_type type = member_type::field;

            explicit constexpr rflite_impl(T C::*) { }

            template <typename U>
            static constexpr decltype(auto) map(U &&ins, T C::*ptr) noexcept {
                return ins.*ptr;
            }

            template <typename... _>
            static constexpr int invoke(_ &&...) noexcept {
                static_assert(sizeof...(_) != sizeof...(_), "variables can't be invoked");
                return 0;
            }
        };

        template <typename T, typename C, typename... Ts>
        struct rflite_impl<T (C::*)(Ts...)> {
            using value_t = T;
            using class_t = C;
            using args_t  = ::std::tuple<Ts...>;

            static constexpr member_type type = member_type::function;

            explicit constexpr rflite_impl(T (C::*)(Ts...)) { }

            template <typename... _>
            static constexpr int map(_ &&...) noexcept {
                static_assert(sizeof...(_) != sizeof...(_), "functions can't be mapped");
                return 0;
            }

            template <typename... Us>
            static constexpr decltype(auto) invoke(T (C::*ptr)(Ts...), C &ins, Us &&... us) {
                return (ins.*ptr)(::std::forward<Us>(us)...);
            }
        };

        template <typename T, typename C, typename... Ts>
        struct rflite_impl<T (C::*)(Ts...) const> {
            using value_t = T;
            using class_t = C;
            using args_t  = ::std::tuple<Ts...>;

            static constexpr member_type type = member_type::function_const;

            explicit constexpr rflite_impl(T (C::*)(Ts...) const) { }

            template <typename... _>
            static constexpr int map(_ &&...) noexcept {
                static_assert(sizeof...(_) != sizeof...(_), "functions can't be mapped");
                return 0;
            }

            template <typename... Us>
            static constexpr decltype(auto) invoke(T (C::*ptr)(Ts...) const, const C &ins, Us &&... us) {
                return (ins.*ptr)(::std::forward<Us>(us)...);
            }
        };

        template <typename T, typename... Ts>
        struct rflite_impl<T (*)(Ts...)> {
            using value_t = T;
            using args_t  = ::std::tuple<Ts...>;

            static constexpr member_type type = member_type::function_static;

            explicit constexpr rflite_impl(T (*)(Ts...)) { }

            template <typename... _>
            static constexpr int map(_ &&...) noexcept {
                static_assert(sizeof...(_) != sizeof...(_), "functions can't be mapped");
                return 0;
            }

            template <typename... Us>
            static constexpr decltype(auto) invoke(T (*ptr)(Ts...), Us &&... us) {
                return ptr(::std::forward<Us>(us)...);
            }
        };

        template <typename T>
        using member_ptr_value_t = typename rflite_impl<T>::value_t;

        template <typename T>
        using member_ptr_class_t = typename rflite_impl<T>::class_t;

        template <typename T>
        using func_ptr_args_t = typename rflite_impl<T>::args_t;

        template <typename T>
        static constexpr size_t func_ptr_args_count_v = ::std::tuple_size_v<func_ptr_args_t<T>>;

        template <typename T>
        static constexpr size_t meta_id_v = T::template RFLITE_META_INFO_NULL::get_id();

        template <typename T>
        struct none_meta { };

        template <typename T>
        struct none_meta_traits {
            static constexpr bool value = false;

            using type = T;
        };

        template <typename T>
        struct none_meta_traits<none_meta<T>> {
            static constexpr bool value = true;

            using type = T;
        };

        template <template <size_t, typename> typename>
        class has_meta_impl { };

        template <typename T>
        concept has_meta = requires { typename has_meta_impl<T::template RFLITE_META_INFO_NAME>; };

        template <typename T>
        struct base_of {
            using type = T;
        };

        // Using `has_meta` may incur that `has_meta` is solved to wrong value
        // Don't know whether it's by design or a bug of clang 10.0 :(
        template <typename T>
        requires requires { typename has_meta_impl<T::template RFLITE_META_INFO_NAME>; }
        struct base_of<T> {
            using type = typename T::template RFLITE_META_INFO_NULL::owner_t;
        };

        template <typename T>
        using base_of_t = typename base_of<T>::type;

        template <typename T>
        struct meta_of_impl {
            using type = none_meta<T>;
        };

        template <has_meta T>
        struct meta_of_impl<T> {
            using type = typename T::template RFLITE_META_INFO_NAME<meta_id_v<T>>;
        };

        template <typename... Ts>
        struct meta_of_foreach;
    }  // namespace impl

    template <typename T>
    static constexpr bool is_none_meta_v = RFLITE_IMPL none_meta_traits<T>::value;

    template <typename T>
    using none_meta_t = typename RFLITE_IMPL none_meta_traits<T>::type;

    template <typename T>
    using meta_of = typename RFLITE_IMPL meta_of_impl<T>::type;

    template <typename T>
    using meta_d_of = meta_of<::std::decay_t<T>>;

    template <typename T>
    using base_of = typename meta_of<T>::base_t;

    template <typename T>
    static constexpr bool is_base_v = ::std::is_same_v<T, base_of<T>>;

    template <typename T>
    static constexpr member_type member_ptr_type_v = RFLITE_IMPL rflite_impl<::std::remove_reference_t<T>>::type;

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

    template <typename Fn, typename... Ts>
    constexpr auto meta_of_foreach(Fn &&fn) {
        return RFLITE_IMPL meta_of_foreach<Ts...>(::std::forward<Fn>(fn));
    }

    template <auto Val, auto... Vals>
    class index_of_first {
        template <size_t I, auto V>
        static constexpr size_t find() {
            return I;
        }

        template <size_t I, auto V, auto V0, auto... Vs>
        static constexpr size_t find() {
            return V != V0 ? find<I + 1, V>(Vs...) : I;
        }

      public:
        static constexpr size_t value = find<0, Val, Vals...>();
    };

    template <auto Val, auto... Vals>
    static constexpr size_t index_of_first_v = index_of_first<Val, Vals...>::value;

    template <typename T, typename... Ts>
    static constexpr size_t index_of_first_t_v = index_of_first_v<true, ::std::is_same_v<T, Ts>...>;

    template <typename T>
    struct tuple_has;

    template <typename... Ts>
    struct tuple_has<::std::tuple<Ts...>> {
        template <typename T>
        static constexpr bool value = (::std::is_same_v<T, Ts> || ...);
    };

    template <typename T, typename TTuple>
    static constexpr bool tuple_has_v = tuple_has<TTuple>::template value<T>;

    template <size_t ILo, size_t ILen, typename... Ts>
    class sub_type_pack {
        template <size_t Lo, size_t... Is>
        static constexpr auto sub_pack(::std::index_sequence<Is...>) noexcept {
            return ::std::tuple<::std::tuple_element_t<Lo + Is, ::std::tuple<Ts...>>...>();
        }

      public:
        using type = decltype(sub_pack<ILo>(::std::make_index_sequence<ILen>()));
    };

    template <size_t Lo, size_t Len, typename... Ts>
    using sub_type_pack_t = typename sub_type_pack<Lo, Len, Ts...>::type;

    template <size_t Lo, size_t Len, typename T, T... Vals>
    class sub_val_pack {
        template <size_t... Is>
        static constexpr auto sub_pack(::std::index_sequence<Is...>) noexcept {
            return ::std::integer_sequence<T, ::std::get<Lo + Is>(::std::integer_sequence<T, Vals...>())...>();
        }

      public:
        using type = decltype(sub_pack(::std::make_index_sequence<Len>()));
    };

    template <size_t Lo, size_t Len, typename T, T... Vals>
    using sub_val_pack_t = typename sub_val_pack<Lo, Len, T, Vals...>::type;

    template <typename T, bool... Vals>
    class mask_tuple {
        template <typename TTuple, typename... Ts, bool... Vs>
        static constexpr auto mask(::std::tuple<Ts...>, ::std::integer_sequence<bool, Vs...>) {
            static_assert(sizeof...(Ts) == sizeof...(Vs));

            constexpr size_t index = index_of_first_v<true, Vs...>;

            if constexpr (index < sizeof...(Ts)) {
                using truet = sub_type_pack_t<index, 1, Ts...>;
                using subt  = sub_type_pack_t<index + 1, sizeof...(Ts) - index - 1, Ts...>;
                using subv  = sub_val_pack_t<index + 1, sizeof...(Vs) - index - 1, bool, Vs...>;

                return mask<decltype(::std::tuple_cat(TTuple(), ::std::tuple<truet>()))>(subt(), subv());
            }
            else
                return TTuple();
        }

        template <typename... Ts>
        static constexpr auto mask(::std::tuple<Ts...>) {
            return mask<::std::tuple<>>(::std::tuple<Ts...>(), ::std::integer_sequence<bool, Vals...>());
        }

      public:
        using type = decltype(mask(T()));
    };

    template <typename TTuple, bool... Vals>
    using mask_tuple_t = typename mask_tuple<TTuple, Vals...>::type;

    template <size_t N, template <typename...> typename TT, typename... Ts>
    class bind_traits {
        template <typename... Us>
        struct impl;

        template <typename T, typename... Tslo, typename... Tshi>
        struct impl<T, ::std::tuple<Tslo...>, ::std::tuple<Tshi...>> {
            using type = TT<Tslo..., T, Tshi...>;
        };

      public:
        template <typename T>
        struct type {
            static constexpr auto value = impl<T, sub_type_pack_t<0, N, Ts...>, sub_type_pack_t<N, sizeof...(Ts) - N, Ts...>>::type::value;
        };
    };

    namespace impl {
        template <typename T>
        struct is_specialization_of_impl {
            template <template <typename...> typename U>
            static constexpr bool value = false;
        };

        template <template <typename...> typename T, typename... Ts>
        struct is_specialization_of_impl<T<Ts...>> {
            template <template <typename...> typename U>
            static constexpr bool value = ::std::is_same_v<T<Ts...>, U<Ts...>>;
        };
    }  // namespace impl

    template <template <typename...> typename T, typename TSpec>
    concept is_specialization_of = impl::is_specialization_of_impl<TSpec>::template value<T>;

    namespace impl {
        template <template <typename> typename>
        class typed_attribute_impl { };
    }  // namespace impl

    template <typename T>
    concept typed_attribute = requires { typename impl::typed_attribute_impl<T::template typed>; };

    struct attribute_tag {
        template <typename T>
        static uintptr_t get_id() requires ::std::is_base_of_v<attribute_tag, T> {
            return static_cast<T *>(nullptr)->get_id();
        }

        virtual uintptr_t get_id() const = 0;
    };

    template <typename T>
    struct attribute : attribute_tag {
        uintptr_t get_id() const override final {
            static T *placeholder;
            return reinterpret_cast<uintptr_t>(&placeholder);
        }

        template <typename TOwner = void>
        constexpr decltype(auto) get() const {
            return get_impl<TOwner>();
        }

      private:
        template <typename>
        constexpr const T &get_impl() const {
            return *static_cast<const T *>(this);
        }

        template <typed_attribute TOwner>
        constexpr decltype(auto) get_impl() const {
            return get_impl<T::template typed, TOwner>();
        }

        template <template <typename> typename TTyped, typename TOwner>
        constexpr TTyped<TOwner> get_impl() const {
            return TTyped<TOwner>();
        }

        template <template <typename> typename TTyped, ::std::constructible_from<const T &> TOwner>
        constexpr TTyped<TOwner> get_impl() const {
            return TTyped<TOwner>(static_cast<const T &>(*this));
        }
    };

    template <typename T>
    static constexpr bool is_attribute_v = ::std::is_base_of_v<attribute<T>, T>;

    struct note_a : attribute<note_a> {
        const ::std::string_view note;

        constexpr note_a(::std::string_view note) : note(note) { }
    };

    struct name_a : attribute<name_a> {
        const ::std::string_view name;

        constexpr name_a(::std::string_view name) : name(name) { }
    };

    template <typename T>
    struct default_a : attribute<default_a<T>> {
        const T value;

        constexpr default_a(const T &value) : value(value) { }
    };

    default_a(const char *)->default_a<::std::string_view>;

    template <typename... TArgs>
    struct ctor_a : attribute<ctor_a<TArgs...>> {
        template <typename T>
        struct typed {
            using args_t = ::std::tuple<TArgs...>;

            template <typename... Args>
            static constexpr T construct(Args &&... args) requires ::std::invocable<void(TArgs...), Args &&...> {
                return T(::std::forward<Args>(args)...);
            }
        };
    };

    template <typename TRet, typename... TArgs>
    struct ctor_a<TRet(TArgs...)> : attribute<ctor_a<TRet(TArgs...)>> {
        using args_t = ::std::tuple<TArgs...>;

        explicit constexpr ctor_a(TRet (*ctor)(TArgs...)) : _ctor(ctor) { }

        template <typename... Args>
        constexpr TRet construct(Args &&... args) const requires ::std::invocable<void(TArgs...), Args &&...> {
            return _ctor(::std::forward<Args>(args)...);
        }

      private:
        TRet (*const _ctor)(TArgs...);
    };

    template <typename T>
    struct ctor_a<T, decltype(&T::operator())> : attribute<ctor_a<T, decltype(&T::operator())>> {
        using args_t = RFLITE_IMPL func_ptr_args_t<decltype(&T::operator())>;

        template <typename U>
        explicit constexpr ctor_a(U &&ctor) : _ctor(::std::forward<U>(ctor)) { }

        template <typename... Args>
        constexpr decltype(auto) construct(Args &&... args) const
            requires ::std::convertible_to<::std::tuple<Args &&...>, args_t> {
            return _ctor(::std::forward<Args>(args)...);
        }

      private:
        const T _ctor;
    };

    template <typename TRet, typename... TArgs>
    ctor_a(TRet (*)(TArgs...)) -> ctor_a<TRet(TArgs...)>;

    template <typename T>
    ctor_a(T &&t) -> ctor_a<T, decltype(&T::operator())>;
}  // namespace rflite

#pragma region helper macro

#define GetMemberMeta(type, member)                                        \
    ([&]() constexpr {                                                     \
        constexpr size_t id = RFLITE meta_of<type>::get_meta_id(member);   \
        static_assert(id != RFLITE_META_INFO_NULL_ID, "Member not found"); \
        return type::RFLITE_META_INFO_NAME<id>();                          \
    }())

#pragma endregion
#pragma endregion

#pragma region dynamic reflection
#ifdef RFLITE_DYNAMIC

#include <memory>
#include <unordered_map>
#include <vector>

namespace rflite::impl {
    template <typename T>
    using allocator_type = ::std::allocator<T>;

    template <typename T>
    static inline allocator_type<T> get_allocator() {
        return allocator_type<T>();
    }

    template <typename T>
    static inline auto get_allocator_of_type() {
        return typename T::allocator_type();
    }

    template <typename T>
    static inline T *allocate(allocator_type<T> &alloc, size_t count) noexcept {
        return ::std::allocator_traits<allocator_type<T>>::allocate(alloc, count);
    }

    template <typename T, typename... Ts>
    static inline void construct(allocator_type<T> &alloc, T *ptr, Ts &&... ts) noexcept {
        ::std::allocator_traits<allocator_type<T>>::construct(alloc, ptr, ::std::forward<Ts>(ts)...);
    }

    template <typename T>
    static inline void destroy(allocator_type<T> &alloc, T *ptr) noexcept {
        ::std::allocator_traits<allocator_type<T>>::destroy(alloc, ptr);
    }

    template <typename T>
    static inline void deallocate(allocator_type<T> &alloc, T *ptr, size_t count) noexcept {
        ::std::allocator_traits<allocator_type<T>>::deallocate(alloc, ptr, count);
    }

    static inline void *allocate(allocator_type<char> &alloc, size_t size, size_t align, size_t count) noexcept {
        size_t space = size * count;
        size_t total = space + align - 1;

        char *p  = allocate(alloc, total + sizeof(size_t));
        void *ap = p + sizeof(size_t);
        if ((ap = ::std::align(align, space, ap, total))) {
            static_cast<size_t *>(ap)[-1] = static_cast<char *>(ap) - p;
            return ap;
        }

        deallocate(alloc, p, total + sizeof(size_t));
        return nullptr;
    }

    static inline void deallocate(allocator_type<char> &alloc, void *ptr, size_t size, size_t align, size_t count) noexcept {
        if (!ptr) return;

        size_t offset = static_cast<size_t *>(ptr)[-1];
        deallocate(alloc, static_cast<char *>(ptr) - offset, size * count + align - 1 + sizeof(size_t));
    }

    template <typename T>
    static constexpr T max(const T &t) {
        return t;
    }

    template <typename T, typename... Ts>
    static constexpr T max(const T &t0, const T &t1, const Ts &... ts) {
        return max((t0 < t1) ? t1 : t0, ts...);
    }

    class any_ptr {
        struct _ { };

        mutable char _buf[max(sizeof(_ *), sizeof(_ (*)()), sizeof(_ _::*), sizeof(_ (_::*)()), sizeof(_ (_::*)() const))];

      public:
        template <typename T>
        any_ptr(T &&ptr) {
            *reinterpret_cast<T *>(_buf) = ptr;
        }

        template <typename T>
        T cast() const {
            return *reinterpret_cast<T *>(_buf);
        }
    };
}  // namespace rflite::impl

namespace rflite {
    class refl_class;
    class refl_instance;

    using refl_map_t  = ::std::unordered_map<::std::string_view, refl_class>;
    using refl_iter_t = typename refl_map_t::iterator;

    using refl_child_collection_t = ::std::vector<::std::string_view>;

    class refl_attr_collection {
        friend class refl_class;
        friend class refl_member;

        static inline RFLITE_IMPL allocator_type<attribute_tag *> Alloc = RFLITE_IMPL get_allocator<attribute_tag *>();

        attribute_tag **_attrs;

        const size_t _nattr;

        template <typename... Ts, size_t... Is>
        refl_attr_collection(const ::std::tuple<Ts...> &t, ::std::index_sequence<Is...>)
            : _attrs(sizeof...(Is) > 0 ? RFLITE_IMPL allocate(Alloc, sizeof...(Ts)) : nullptr), _nattr(sizeof...(Ts)) {
            ((*const_cast<const attribute_tag **>(&_attrs[Is]) = &::std::get<Is>(t)), ...);
        }

        template <typename... Ts>
        refl_attr_collection(const ::std::tuple<Ts...> &t)
            : refl_attr_collection(t, ::std::index_sequence_for<Ts...>()) {
        }

      public:
        refl_attr_collection(const refl_attr_collection &) = delete;
        refl_attr_collection(refl_attr_collection &&o) : _attrs(o._attrs), _nattr(o._nattr) {
            o._attrs = nullptr;
        }

        ~refl_attr_collection() {
            if (_attrs)
                RFLITE_IMPL deallocate(Alloc, _attrs, _nattr);
        }

        template <typename T>
        const T &get() const requires is_attribute_v<T> {
            size_t id = attribute_tag::template get_id<T>();
            for (size_t i = 0; i < _nattr; i++)
                if (_attrs[i]->get_id() == id)
                    return *static_cast<T *>(_attrs[i]);
            throw;
        }
    };

    class refl_member {
        template <typename T, ::std::enable_if_t<!::std::is_reference_v<T>, int> = 0>
        static constexpr decltype(auto) cast_arg_ref(void *ptr) {
            return *reinterpret_cast<T *>(ptr);
        }

        template <typename T, ::std::enable_if_t<::std::is_reference_v<T>, int> = 0>
        static constexpr decltype(auto) cast_arg_ref(void *ptr) {
            return static_cast<T>(*reinterpret_cast<::std::remove_reference_t<T> *>(ptr));
        }

        template <typename T>
        static constexpr decltype(auto) cast_res_ref(void *ptr) {
            return *reinterpret_cast<::std::remove_reference_t<RFLITE_IMPL member_ptr_value_t<T>> *>(ptr);
        }

        template <typename T>
        static constexpr decltype(auto) cast_class_ref(void *ptr) {
            return cast_arg_ref<RFLITE_IMPL member_ptr_class_t<T>>(ptr);
        }

        template <typename T>
        static void access_field(void *res, void **args, const RFLITE_IMPL any_ptr &ptr) {
            *reinterpret_cast<RFLITE_IMPL member_ptr_value_t<T> **>(res) = &(cast_class_ref<T>(*args).*(ptr.cast<T>()));
        }

        template <typename T>
        static void access_field_static(void *res, void **args, const RFLITE_IMPL any_ptr &ptr) {
            *reinterpret_cast<T *>(res) = ptr.cast<T>();
        }

        template <typename T, typename TTuple, size_t... Is>
        struct access_func_static {
            static void impl(void *res, void **args, const RFLITE_IMPL any_ptr &ptr) {
                cast_res_ref<T>(res) = ptr.cast<T>()(cast_arg_ref<::std::tuple_element_t<Is, TTuple>>(args[Is])...);
            }
        };

        template <typename T, typename TTuple, size_t... Is>
        struct access_func_member {
            static void impl(void *res, void **args, const RFLITE_IMPL any_ptr &ptr) {
                cast_res_ref<T>(res) = (cast_class_ref<T>(*args).*ptr.cast<T>())(cast_arg_ref<::std::tuple_element_t<Is, TTuple>>(args[Is + 1])...);
            }
        };

        template <typename T, typename TTuple, size_t... Is>
        struct access_func_member_const {
            static void impl(void *res, void **args, const RFLITE_IMPL any_ptr &ptr) {
                cast_res_ref<T>(res) = (cast_class_ref<T>(*args).*ptr.cast<T>())(cast_arg_ref<::std::tuple_element_t<Is, TTuple>>(args[Is + 1])...);
            }
        };

        template <template <typename, typename, size_t...> typename TAccess, typename T, typename TTuple, size_t... Is>
        static constexpr auto access(::std::index_sequence<Is...>) -> void (*)(void *, void **, const RFLITE_IMPL any_ptr &) {
            return &TAccess<T, TTuple, Is...>::impl;
        }

        template <typename T>
        static auto access() -> void (*)(void *, void **, const RFLITE_IMPL any_ptr &) {
            constexpr member_type type = member_ptr_type_v<T>;
            if constexpr (type == member_type::field)
                return &access_field<T>;
            if constexpr (type == member_type::field_static)
                return &access_field_static<T>;
            if constexpr (type == member_type::function_static)
                return access<access_func_static, T, RFLITE_IMPL func_ptr_args_t<T>>(::std::make_index_sequence<RFLITE_IMPL func_ptr_args_count_v<T>>());
            if constexpr (type == member_type::function)
                return access<access_func_member, T, RFLITE_IMPL func_ptr_args_t<T>>(::std::make_index_sequence<RFLITE_IMPL func_ptr_args_count_v<T>>());
            if constexpr (type == member_type::function_const)
                return access<access_func_member_const, T, RFLITE_IMPL func_ptr_args_t<T>>(::std::make_index_sequence<RFLITE_IMPL func_ptr_args_count_v<T>>());
            return nullptr;
        }

        const ::std::string_view _name;

        refl_attr_collection _attrs;

        const member_type _memberType;

        const size_t _size;

        const RFLITE_IMPL any_ptr _memberPtr;

        // when _memberType is:
        // field: return pointer to the member of instance
        // function_xxx: return invocation result of the function
        void (*const _handler)(void *, void **, const RFLITE_IMPL any_ptr &);

      public:
        template <typename T>
        refl_member(T &&meta)
            : _name(meta.member_name()),
              _attrs(meta.get_attr_all()),
              _memberType(member_ptr_type_v<decltype(meta.member_ptr())>),
              _size(sizeof(RFLITE_IMPL member_ptr_value_t<decltype(meta.member_ptr())>)),
              _memberPtr(meta.member_ptr()),
              _handler(access<decltype(meta.member_ptr())>()) { }

        ::std::string_view name() const {
            return _name;
        }

        template <typename T>
        const T &get_attr() const requires is_attribute_v<T> {
            return _attrs.get<T>();
        }

        size_t size() const {
            return _size;
        }

        member_type type() const {
            return _memberType;
        }

        bool is_static() const {
            return !has_flag(member_type::any_static, _memberType);
        }

        void get(const void *ins, void *buf) const {
            assert_not_func();

            void *mp, *mut = const_cast<void *>(ins);
            _handler(&mp, &mut, _memberPtr);
            copy_impl(mp, buf);
        }

        void set(void *ins, void *buf) const {
            assert_not_func();

            void *mp;
            _handler(&mp, &ins, _memberPtr);
            copy_impl(buf, mp);
        }

        template <typename T>
        T &of(void *ins) const {
            assert_not_func();

            void *mp;
            _handler(&mp, &ins, _memberPtr);
            return *reinterpret_cast<T *>(mp);
        }

        template <typename T>
        T &of() const {
            assert(_memberType == member_type::field_static);

            void *mp;
            _handler(&mp, nullptr, _memberPtr);
            return *reinterpret_cast<T *>(mp);
        }

        template <typename T>
        const T &of(const void *ins) const {
            assert_not_func();

            void *mp, *mut = const_cast<void *>(ins);
            _handler(&mp, &ins, _memberPtr);
            return *reinterpret_cast<const T *>(mp);
        }

        template <typename... Ts>
        void invoke(void *buf, Ts &&... args) const {
            invoke_impl(buf, ::std::index_sequence_for<Ts...>(), ::std::forward<Ts>(args)...);
        }

      private:
        void assert_not_func() const {
            assert(!is_function(_memberType));
        }

        void assert_is_func() const {
            assert(is_function(_memberType));
        }

        void copy_impl(void *src, void *dest) const {
            ::std::copy(static_cast<char *>(src), static_cast<char *>(src) + _size, static_cast<char *>(dest));
        }

        template <typename... Ts, size_t... Is>
        void invoke_impl(void *buf, ::std::index_sequence<Is...>, Ts &&... args) const {
            assert_is_func();

            void *paramp[sizeof...(Ts)] { (&args)... };
            _handler(buf, paramp, _memberPtr);
        }
    };

    class refl_member_view {
        friend class refl_class_view;

        const refl_member &_meta;

        void *_ins;

        refl_member_view(const refl_member &meta, void *ins)
            : _meta(meta), _ins(ins) { }

      public:
        refl_member_view &operator<<(void *buf) {
            _meta.set(_ins, buf);
            return *this;
        }

        const refl_member_view &operator>>(void *buf) const {
            _meta.get(_ins, buf);
            return *this;
        }

        template <typename T>
        T &as() {
            return _meta.of<T>(_ins);
        }

        template <typename T>
        const T &as() const {
            return _meta.of<T>(const_cast<const void *>(_ins));
        }

        template <typename... Ts>
        void invoke(void *buf, Ts &&... args) {
            if (has_flag(_meta.type(), member_type::function_this)) {
                assert(_ins);
                _meta.invoke(buf, _ins, ::std::forward<Ts>(args)...);
            }
            else
                _meta.invoke(buf, ::std::forward<Ts>(args)...);
        }
    };

    class refl_class {
        friend class refl_table;

        static inline RFLITE_IMPL allocator_type<char> Alloc;

        const ::std::string_view _name;

        const size_t _size;

        const size_t _align;

        refl_attr_collection _attrs;

        ::std::shared_ptr<refl_member[]> _members;

        const size_t _nmember;

        const bool _isBase;

        const refl_iter_t _baseIt;

        refl_child_collection_t _childs;

        template <typename TAttrTuple>
        refl_class(::std::string_view name, size_t size, size_t align, const TAttrTuple &attrs,
                   ::std::shared_ptr<refl_member[]> &&members, size_t nmember,
                   bool isBase, const refl_iter_t &baseIt)
            : _name(name), _size(size), _align(align), _attrs(attrs),
              _members(members), _nmember(nmember),
              _isBase(isBase), _baseIt(baseIt),
              _childs(RFLITE_IMPL get_allocator_of_type<refl_child_collection_t>()) {
            if (!isBase)
                baseIt->second._childs.push_back(name);
        }

      public:
        refl_class(const refl_class &) = delete;
        refl_class(refl_class &&)      = default;

        ::std::string_view name() const {
            return _name;
        }

        size_t size() const {
            return _size;
        }

        size_t align() const {
            return _align;
        }

        template <typename T>
        const T &get_attr() const requires is_attribute_v<T> {
            return _attrs.get<T>();
        }

        const refl_class &base() const {
            if (_isBase)
                return *this;
            else
                return _baseIt->second;
        }

        const refl_member *begin() const {
            return &_members[0];
        }

        const refl_member *end() const {
            return &_members[_nmember];
        }

        size_t member_count() const {
            return _nmember;
        }

        refl_class_view view(void *buf) const;

        refl_instance make(size_t n = 1) const;

        template <typename TInsIt>
        size_t childs(TInsIt &&it) const;

        const refl_member &operator[](size_t index) const {
            assert(index < _nmember);
            return _members[index];
        }

        const refl_member &operator[](::std::string_view member) const {
            for (const refl_member &m : *this)
                if (m.name() == member)
                    return m;
            throw;
        }

        bool operator==(const refl_class &r) const {
            return &r == this;
        }
    };

    class refl_class_view {
        friend class refl_class;

        const refl_class &_meta;

        void *_ins;

        refl_class_view(const refl_class &meta, void *ins)
            : _meta(meta), _ins(ins) { }

      public:
        class iterator : ::std::iterator_traits<refl_member_view *> {
            friend class refl_class_view;

            const refl_member *_refl;

            void *_ins;

            iterator(const refl_member *refl, void *ins) : _refl(refl), _ins(ins) { }

          public:
            refl_member_view operator*() const {
                return refl_member_view(*_refl, _ins);
            }

            iterator &operator++() {
                ++_refl;
                return *this;
            }

            bool operator!=(const iterator &o) const {
                return _ins != o._ins || _refl != o._refl;
            }
        };

        const refl_class &meta() const {
            return _meta;
        }

        iterator begin() const {
            return iterator(_meta.begin(), _ins);
        }

        iterator end() const {
            return iterator(_meta.end(), _ins);
        }

        template <typename T>
        T &as() {
            return *static_cast<T *>(_ins);
        }

        template <typename T>
        const T &as() const {
            return *static_cast<T *>(_ins);
        }

        refl_member_view operator[](size_t index) const {
            return refl_member_view(_meta[index], _ins);
        }

        refl_member_view operator[](::std::string_view member) const {
            return refl_member_view(_meta[member], _ins);
        }
    };

    class refl_instance {
        friend class refl_class;

        const refl_class &_refl;

        const size_t _size;

        const size_t _count;

        ::std::shared_ptr<void> _ins;

        refl_instance(const refl_class &refl, ::std::shared_ptr<void> ins, size_t count)
            : _refl(refl), _size(refl.size()), _count(count), _ins(::std::move(ins)) { }

      public:
        refl_instance(const refl_instance &o) = delete;
        refl_instance(refl_instance &&o)      = default;

        class iterator : ::std::iterator_traits<void *> {
            friend class refl_instance;

            const refl_class &_refl;

            void *_ins;

            size_t _size;

            iterator(const refl_class &refl, void *ins, size_t size)
                : _refl(refl), _ins(ins), _size(size) { }

          public:
            refl_class_view operator*() const {
                return _refl.view(_ins);
            }

            iterator &operator++() {
                _ins = static_cast<char *>(_ins) + _size;
                return *this;
            }

            bool operator!=(const iterator &o) const {
                return _ins != o._ins || &_refl != &o._refl;
            }
        };

        iterator begin() const {
            return iterator(_refl, at(0), _size);
        }

        iterator end() const {
            return iterator(_refl, at(_count), _size);
        }

        size_t count() const {
            return _count;
        }

        refl_class_view operator[](size_t index) const {
            assert(index < _count);
            return _refl.view(at(index));
        }

      private:
        void *at(size_t index) const {
            return static_cast<char *>(_ins.get()) + index * _size;
        }
    };

    class refl_table {
        static inline RFLITE_IMPL allocator_type<refl_member> Alloc = RFLITE_IMPL get_allocator<refl_member>();

        static inline refl_map_t ClassMap { RFLITE_IMPL get_allocator_of_type<refl_map_t>() };

      public:
        template <typename T>
        static refl_iter_t regist() {
            if constexpr (meta_of<T>::template has_attr<name_a>())
                return regist<T>(meta_of<T>::template get_attr<name_a>().name);
            else
                return regist<T>(meta_of<T>::name());
        }

        template <typename T>
        static refl_iter_t regist(::std::string_view name) {
            if (is_registered(name))
                return ClassMap.find(name);

            refl_iter_t base;
            if constexpr (!is_base_v<T>)
                base = regist<base_of<T>>();

            constexpr size_t nrefl = meta_of<T>::get_meta_count();

            ::std::shared_ptr<refl_member[]> members(
                nrefl ? RFLITE_IMPL allocate(Alloc, nrefl) : nullptr,
                [=](refl_member *p) {
                    if constexpr (nrefl > 0) {
                        RFLITE_IMPL destroy(Alloc, p);
                        RFLITE_IMPL deallocate(Alloc, p, nrefl);
                    }
                });

            size_t i = 0;
            meta_of<T>::template foreach<member_type::any>([&](auto &&meta) {
                RFLITE_IMPL construct(Alloc, &members[i++], meta);
            });

            auto [it, res] = ClassMap.emplace(name, refl_class(name, sizeof(T), alignof(T), meta_of<T>::get_attr_all(), ::std::move(members), nrefl, is_base_v<T>, base));
            if (res)
                return it;
            throw;
        }

        static const refl_class &get_class(::std::string_view name) {
            return ClassMap.at(name);
        }

      private:
        static bool is_registered(::std::string_view name) {
            return ClassMap.contains(name);
        }
    };

    inline refl_class_view refl_class::view(void *buf) const {
        return refl_class_view(*this, buf);
    }

    inline refl_instance refl_class::make(size_t n) const {
        return refl_instance(*this,
                             ::std::shared_ptr<void>(
                                 RFLITE_IMPL allocate(Alloc, size(), align(), n),
                                 [=](void *ptr) { RFLITE_IMPL deallocate(Alloc, ptr, _size, _align, n); }),
                             n);
    }

    template <typename TInsIt>
    size_t refl_class::childs(TInsIt &&it) const {
        for (::std::string_view c : _childs)
            *it++ = &refl_table::get_class(c);
        return _childs.size();
    }
}  // namespace rflite

#define META_RT_REGIST static inline const RFLITE refl_iter_t meta_info_rt = RFLITE refl_table::regist<typename RFLITE_META_INFO_NULL::owner_t>();

#define META_E_RT \
    META_E        \
    META_RT_REGIST

#define META_EMPTY_RT(type, ...)  \
    META_EMPTY(type, __VA_ARGS__) \
    META_RT_REGIST

#endif
#pragma endregion
