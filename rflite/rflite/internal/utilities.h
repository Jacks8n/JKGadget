#pragma once

#ifndef RFLITE_PREPROCESS_ONLY
#include <tuple>
#endif

#include "rflite/internal/macro.h"

RFLITE_NS {
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
}

RFLITE_IMPL_NS {
    struct _ { };

    enum class meta_type : size_t { null,
                                    entry,
                                    end };

    template <typename T>
    struct member_traits;

    template <typename T>
    struct member_traits<T *> {
        using value_t = T;

        static constexpr member_type type = member_type::field_static;

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
    struct member_traits<T C::*> {
        using value_t = T;
        using class_t = C;

        static constexpr member_type type = member_type::field;

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
    struct member_traits<T (C::*)(Ts...)> {
        using value_t = T;
        using class_t = C;
        using args_t  = ::std::tuple<Ts...>;

        static constexpr member_type type = member_type::function;

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
    struct member_traits<T (C::*)(Ts...) const> {
        using value_t = T;
        using class_t = C;
        using args_t  = ::std::tuple<Ts...>;

        static constexpr member_type type = member_type::function_const;

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
    struct member_traits<T (*)(Ts...)> {
        using value_t = T;
        using args_t  = ::std::tuple<Ts...>;

        static constexpr member_type type = member_type::function_static;

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
    using member_ptr_value_t = typename member_traits<T>::value_t;

    template <typename T>
    using member_ptr_class_t = typename member_traits<T>::class_t;

    template <typename T>
    using func_ptr_args_t = typename member_traits<T>::args_t;

    template <typename T>
    static constexpr size_t func_ptr_args_count_v = ::std::tuple_size_v<func_ptr_args_t<T>>;

    template <typename T>
    static constexpr member_type member_ptr_type_v = member_traits<::std::remove_reference_t<T>>::type;

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
}

RFLITE_NS {
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
            static constexpr auto value = impl<T, RFLITE_IMPL sub_type_pack_t<0, N, Ts...>, RFLITE_IMPL sub_type_pack_t<N, sizeof...(Ts) - N, Ts...>>::type::value;
        };
    };
}