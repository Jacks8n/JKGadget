#pragma once

#ifndef RFLITE_PREPROCESS_ONLY
#include <concepts>
#include <string_view>
#include <tuple>
#endif

#include "rflite/internal/utilities.h"

#define RFLITE_META_ATTRIBUTE meta_attribute

RFLITE_NS {
    struct attribute_tag {
        using id_t = uintptr_t;

        template <typename T>
        static id_t get_id() requires ::std::is_base_of_v<attribute_tag, T> {
            struct {
                T value;
            } *temp = nullptr;
            return temp->value.get_id();
        }

        virtual id_t get_id() const = 0;
    };
}

RFLITE_IMPL_NS {
    template <template <typename> typename>
    class typed_attribute_impl { };

    template <typename T>
    concept typed_attribute = requires { typename RFLITE_IMPL typed_attribute_impl<T::template typed>; };

    template <typename>
    struct is_tuple_impl {
        static constexpr bool value = false;
    };

    template <typename... Ts>
    struct is_tuple_impl<::std::tuple<Ts...>> {
        static constexpr bool value = true;
    };

    template <typename T>
    concept is_tuple = is_tuple_impl<T>::value;

    template <typename T>
    struct specialize_typed_attribute {
        template <typename>
        static constexpr const T &get(const T &attr) noexcept {
            return attr;
        }
    };

    template <typed_attribute T>
    struct specialize_typed_attribute<T> {
        template <typename TClass>
        static constexpr auto get(const T &attr) noexcept {
            using typed = typename T::template typed<TClass>;

            struct indexed_typed : RFLITE attribute_tag, typed {
                using typed::typed;

                id_t get_id() const override {
                    return attribute_tag::template get_id<T>();
                }
            };

            if constexpr (::std::constructible_from<typed, const T &>)
                return indexed_typed(attr);
            else
                return indexed_typed();
        }
    };
}

RFLITE_NS {
    template <typename T>
    struct attribute : attribute_tag {
        id_t get_id() const override final {
            static T *placeholder;
            return reinterpret_cast<id_t>(&placeholder);
        }

        template <typename TClass>
        constexpr decltype(auto) get() const noexcept {
            return RFLITE_IMPL specialize_typed_attribute<T>::template get<TClass>(static_cast<const T &>(*this));
        }
    };

    template <typename T>
    concept is_attribute = ::std::is_base_of_v<attribute<T>, T>;

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

    template <typename>
    class func_a;

    template <typename TRet, typename... TArgs>
    class func_a<TRet(TArgs...)> : public attribute<func_a<TRet(TArgs...)>> {
        template <typename>
        struct wrap_ctor {
            template <typename T>
            static constexpr auto get(T &&ptr) noexcept {
                return ptr;
            }
        };

        template <typename T>
        requires ::std::is_default_constructible_v<T> struct wrap_ctor<T> {
            template <typename _>
            static constexpr auto get(_ &&) noexcept {
                return &invoke_ctor;
            }

          private:
            static constexpr TRet invoke_ctor(TArgs... args) {
                return T()(static_cast<TArgs>(args)...);
            }
        };

      public:
        using args_t = ::std::tuple<TArgs...>;

        template <typename T>
        explicit constexpr func_a(T &&ctor) : _ctor(wrap_ctor<::std::remove_cvref_t<T>>::get(ctor)) { }

        template <typename... Args>
        constexpr TRet invoke(Args &&... args) const {
            return _ctor(::std::forward<Args>(args)...);
        }

      private:
        TRet (*const _ctor)(TArgs...);
    };

    template <typename TRet, typename... TArgs>
    func_a(TRet(*)(TArgs...)) -> func_a<TRet(TArgs...)>;

    template <typename T>
    requires requires { typename ::std::void_t<decltype(T::operator())>; }
    func_a(T &&)->func_a<member_ptr_deref_t<decltype(&T::operator())>>;
}

RFLITE_IMPL_NS {
    template <typename T>
    using attr_wrap_t = ::std::conditional_t<is_attribute<T>, T, any_a<T>>;

    template <typename T>
    struct attr_unwrap {
        static constexpr const T &unwrap(const attr_wrap_t<T> &attr) noexcept {
            return attr;
        }
    };

    template <is_attribute T>
    struct attr_unwrap<T> {
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

        explicit constexpr RFLITE_META_ATTRIBUTE(const TTypeds &... ts) : _attributes(::std::make_tuple(ts...)) { }

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
        constexpr const auto &get() const noexcept {
            if constexpr (typed_attribute<T>)
                return ::std::get<T::template typed<TClass>>(all());
            else
                return attr_unwrap<T>::unwrap(::std::get<attr_wrap_t<T>>(all()));
        }

        template <template <typename...> typename T>
        constexpr const auto &get() const noexcept {
            return ::std::get<index_of_first_v<true, is_specialization_of<T, Ts>...>>(all());
        }
    };

    template <typename TClass, typename... Ts>
    static constexpr auto make_attributes(const Ts &... ts) noexcept {
        constexpr auto make_attr = []<typename T>(const T &t) constexpr {
            if constexpr (is_attribute<T>)
                if constexpr (typed_attribute<T>)
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
