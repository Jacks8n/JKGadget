#pragma once

#ifndef RFLITE_PREPROCESS_ONLY
#include <string_view>
#include <tuple>
#include <type_traits>
#endif

#include "rflite/internal/utilities.h"

#define RFLITE_META_ATTRIBUTE meta_attribute

RFLITE_NS {
    struct attribute_tag {
        using attribute_id_t = uintptr_t;

        template <typename T>
        static attribute_id_t get_id() {
            static_assert(::std::is_base_of_v<attribute_tag, T>);

            struct {
                T value;
            } *temp = nullptr;
            return temp->value.get_id();
        }

        virtual attribute_id_t get_id() const = 0;
    };
}

RFLITE_IMPL_NS {
    template <template <typename> typename>
    using if_typed_attribute_template_t = void;

    template <typename T, typename = void>
    struct typed_attribute_impl : ::std::false_type { };

    template <typename T>
    struct typed_attribute_impl<T, if_typed_attribute_template_t<T::template typed>> : ::std::true_type { };

    template <typename T>
    constexpr bool is_typed_attribute_v = typed_attribute_impl<T>::value;

    template <typename>
    struct is_tuple_impl : ::std::false_type { };

    template <typename... Ts>
    struct is_tuple_impl<::std::tuple<Ts...>> : ::std::true_type { };

    template <typename T>
    constexpr bool is_tuple_v = is_tuple_impl<T>::value;

    template <typename T, typename = void>
    struct specialize_typed_attribute {
        template <typename>
        static constexpr const T &get(const T &attr) noexcept {
            return attr;
        }
    };

    template <typename T>
    struct specialize_typed_attribute<T, ::std::enable_if_t<is_typed_attribute_v<T>>> {
        template <typename TClass>
        static constexpr auto get(const T &attr) noexcept {
            using typed = typename T::template typed<TClass>;

            struct specialized : RFLITE attribute_tag, typed {
                using typed::typed;

                attribute_id_t get_id() const override {
                    return attribute_tag::template get_id<T>();
                }
            };

            if constexpr (::std::is_constructible_v<typed, const T &>)
                return specialized(attr);
            else
                return specialized();
        }
    };
}

RFLITE_NS {
    template <typename T>
    struct attribute : attribute_tag {
        attribute_id_t get_id() const override final {
            static T *placeholder = nullptr;
            return reinterpret_cast<attribute_id_t>(&placeholder);
        }

        /// @brief Returns the attribute with specialization if the attribute requires the type of owner class
        template <typename TClass>
        constexpr decltype(auto) get() const noexcept {
            return RFLITE_IMPL specialize_typed_attribute<T>::template get<TClass>(static_cast<const T &>(*this));
        }
    };

    template <typename T>
    constexpr bool is_attribute_v = ::std::is_base_of_v<attribute<T>, T>;

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
            static constexpr TClass construct(Args &&...args) {
                return T(::std::forward<Args>(args)...);
            }
        };
    };

    template <typename>
    class func_a;

    template <typename TRet, typename... TArgs>
    class func_a<TRet(TArgs...)> : public attribute<func_a<TRet(TArgs...)>> {
        using func_ptr_t = TRet (*)(TArgs...);

        template <typename T, typename = void>
        struct as_func_ptr {
            static constexpr func_ptr_t wrap(const T &func) noexcept {
                return func;
            }
        };

        template <typename T>
        struct as_func_ptr<T, ::std::enable_if_t<!::std::is_pointer_v<T> && !::std::is_function_v<T>>> {
            static constexpr func_ptr_t wrap(const T &) noexcept {
                return func;
            }

            static constexpr TRet func(TArgs... args) {
                return T()(::std::move(args)...);
            }
        };

      public:
        using args_t = ::std::tuple<TArgs...>;

        explicit constexpr func_a(func_ptr_t func) : _func(func) { }

        template <typename... Args>
        constexpr TRet invoke(Args &&...args) const {
            return _func(::std::forward<Args>(args)...);
        }

      private:
        const func_ptr_t _func;
    };

    template <typename TRet, typename... TArgs>
    func_a(TRet(*)(TArgs...)) -> func_a<TRet(TArgs...)>;

    template <typename T>
    func_a(const T &) -> func_a<member_ptr_deref_t<decltype(&T::operator())>>;
}

RFLITE_IMPL_NS {
    template <typename T>
    using attr_wrap_t = ::std::conditional_t<is_attribute_v<T>, T, any_a<T>>;

    template <typename T, typename = void>
    struct attr_unwrap {
        static constexpr const T &unwrap(const attr_wrap_t<T> &attr) noexcept {
            return attr;
        }
    };

    template <typename T>
    struct attr_unwrap<T, ::std::enable_if_t<is_attribute_v<T>>> {
        static constexpr const T &unwrap(const T &attr) noexcept {
            return attr;
        }
    };

    template <typename...>
    class RFLITE_META_ATTRIBUTE;

    template <typename TClass, typename... Ts, typename... TTypeds>
    class RFLITE_META_ATTRIBUTE<TClass, ::std::tuple<Ts...>, ::std::tuple<TTypeds...>> {
        template <typename U, typename... Us>
        friend constexpr auto make_attributes(const Us &...) noexcept;

        using attribute_tuple_t = ::std::tuple<TTypeds...>;

        explicit constexpr RFLITE_META_ATTRIBUTE(const TTypeds &...ts) : _attributes(::std::make_tuple(ts...)) { }

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
            return (is_specialization_of_v<T, Ts> || ...);
        }

        template <size_t Nth>
        constexpr const auto &get() const noexcept {
            return ::std::get<Nth>(all());
        }

        template <typename T>
        constexpr const auto &get() const noexcept {
            if constexpr (is_typed_attribute_v<T>)
                return ::std::get<T::template typed<TClass>>(all());
            else
                return attr_unwrap<T>::unwrap(::std::get<attr_wrap_t<T>>(all()));
        }

        template <template <typename...> typename T>
        constexpr const auto &get() const noexcept {
            return ::std::get<index_of_first_v<true, is_specialization_of_v<T, Ts>...>>(all());
        }
    };

    template <typename TClass, typename... Ts>
    static constexpr auto make_attributes(const Ts &...ts) noexcept {
        constexpr auto make_attr = [](auto t) {
            using meta_t = decltype(t);

            if constexpr (is_attribute_v<meta_t>)
                if constexpr (is_typed_attribute_v<meta_t>)
                    return ::std::make_pair(t, t.template get<TClass>());
                else
                    return ::std::make_pair(t, t);
            else
                return ::std::make_pair(any_a(t), any_a(t));
        };

        return RFLITE_META_ATTRIBUTE<TClass, ::std::tuple<decltype(make_attr(ts).first)...>,
                                     ::std::tuple<decltype(make_attr(ts).second)...>>(make_attr(ts).second...);
    }
}

#undef RFLITE_META_ATTRIBUTE
