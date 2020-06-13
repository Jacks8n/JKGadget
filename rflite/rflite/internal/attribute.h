#pragma once

#ifndef RFLITE_PREPROCESS_ONLY
#include <concepts>
#include <string_view>
#include <tuple>
#endif

#include "rflite/internal/utilities.h"

#define RFLITE_META_ATTRIBUTE meta_attribute

RFLITE_IMPL_NS {
    template <template <typename> typename>
    class typed_attribute_impl { };

    template <typename T>
    concept typed_attribute = requires { typename RFLITE_IMPL typed_attribute_impl<T::template typed>; };
}

RFLITE_NS {
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

        template <typename TClass = void>
        constexpr const T &get() const {
            return static_cast<const T &>(*this);
        }
    };

    template <typename T>
    // Duplication of `RFLITE typed_attribute`
    requires requires { typename RFLITE_IMPL typed_attribute_impl<T::template typed>; }
    struct attribute<T> {
        template <typename TClass>
        constexpr decltype(auto) get() const {
            static_assert(::std::is_base_of_v<attribute_tag, T::template typed<TClass>>, "nested template class `typed` must inherit from attribute_tag");

            using typed_t = typename T::template typed<TClass>;
            if constexpr (::std::constructible_from<typed_t, const T &>)
                return typed_t(static_cast<const T &>(*this));
            else
                return typed_t();
        }
    };

    template <typename T>
    static constexpr bool is_attribute_v = ::std::is_base_of_v<attribute<T>, T>;

    template <typename T>
    struct any_a : attribute<any_a<T>> {
        const T value;

        explicit constexpr any_a(const T &value) : value(value) { }

        constexpr operator const T &() const {
            return value;
        }
    };

    template <typename T>
    any_a(const T &) -> any_a<T>;

    any_a(const char *)->any_a<::std::string_view>;

    struct name_a : attribute<name_a> {
        const ::std::string_view name;

        explicit constexpr name_a(::std::string_view name) : name(name) { }
    };

    template <typename... TArgs>
    struct ctor_a : attribute<ctor_a<TArgs...>> {
        template <typename TClass>
        struct typed {
            using args_t = ::std::tuple<TArgs...>;

            template <typename... Args>
            static constexpr TClass construct(Args &&... args) requires ::std::invocable<void(TArgs...), Args &&...> {
                return T(::std::forward<Args>(args)...);
            }
        };
    };

    template <typename TRet, typename... TArgs>
    struct ctor_a<TRet(TArgs...)> : attribute<ctor_a<TRet(TArgs...)>> {
        using args_t = ::std::tuple<TArgs...>;

        explicit constexpr ctor_a(TRet (*ctor)(TArgs...)) : _ctor(ctor) { }

        template <typename... Args>
        constexpr TRet construct(Args &&... args) const requires(::std::invocable<TRet(TArgs...), Args...>) {
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
    ctor_a(TRet(*)(TArgs...)) -> ctor_a<TRet(TArgs...)>;

    template <typename T>
    requires requires { typename ::std::void_t<decltype(T::operator())>; }
    ctor_a(T && t)->ctor_a<T, decltype(&T::operator())>;
}

RFLITE_IMPL_NS {
    template <typename T>
    using as_attr_t = ::std::conditional_t<is_attribute_v<T>, T, any_a<T>>;

    template <auto Val, auto... Vals>
    class index_of_first {
        template <size_t I, auto V>
        static constexpr size_t find() {
            return I;
        }

        template <size_t I, auto V, auto V0, auto... Vs>
        static constexpr size_t find() {
            return V != V0 ? find<I + 1, V, Vs...>() : I;
        }

      public:
        static constexpr size_t value = find<0, Val, Vals...>();
    };

    template <auto Val, auto... Vals>
    static constexpr size_t index_of_first_v = index_of_first<Val, Vals...>::value;

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

    template <template <typename...> typename T, typename TSpec>
    concept is_specialization_of = is_specialization_of_impl<TSpec>::template value<T>;

    template <typename TClass, typename... Ts>
    class RFLITE_META_ATTRIBUTE {
        template <typename U, typename... Us>
        friend constexpr auto make_attributes(const Us &...) noexcept;

        using attribute_tuple_t = ::std::tuple<Ts...>;

        explicit constexpr RFLITE_META_ATTRIBUTE(const Ts &... ts) : _attributes(::std::make_tuple(ts...)) { }

        attribute_tuple_t _attributes;

      public:
        constexpr const attribute_tuple_t &all() const noexcept {
            return _attributes;
        }

        constexpr size_t count() const noexcept {
            return sizeof...(Ts);
        }

        template <typename T>
        constexpr bool has() const noexcept {
            return (::std::is_same_v<T, Ts> || ...);
        }

        template <template <typename...> typename T>
        constexpr bool has() const noexcept {
            return (is_specialization_of<T, Ts> || ...);
        }

        template <size_t Nth>
        constexpr const auto &get() const noexcept {
            return ::std::get<Nth>(all());
        }

        template <typename T>
        constexpr const T &get() const noexcept {
            if constexpr (typed_attribute<T>)
                return ::std::get<T::template typed<TClass>>(all());
            else
                return ::std::get<as_attr_t<T>>(all());
        }

        template <template <typename...> typename T>
        constexpr const auto &get() const noexcept {
            return ::std::get<index_of_first_v<true, (is_specialization_of<T, ::std::remove_reference_t<Ts>>)...>>(all());
        }
    };

    template <typename TClass, typename... Ts>
    static constexpr auto make_attributes(const Ts &... ts) noexcept {
        constexpr auto make_attr = []<typename T>(const T &t) constexpr {
            if constexpr (is_attribute_v<::std::remove_reference_t<T>>)
                return t;
            else
                return any_a(t);
        };

        return RFLITE_META_ATTRIBUTE<TClass, decltype(make_attr(ts))...>(make_attr(ts)...);
    }
}

#undef RFLITE_META_ATTRIBUTE