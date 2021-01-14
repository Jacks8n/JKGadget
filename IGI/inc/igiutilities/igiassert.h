#pragma once

#include <iostream>
#include <utility>

namespace igi {
    template <typename... Ts>
    constexpr void LogError(Ts &&...ts) {
        if (std::is_constant_evaluated())
            return;
        else {
            (std::cerr << ... << ts);
            if constexpr (sizeof...(Ts) > 0)
                std::cerr << std::endl;
        }
    }

    template <typename... Ts>
    constexpr void Assert(bool b, Ts &&...ts) {
        if (std::is_constant_evaluated())
            return;
        else {
            if (b)
                return;

            LogError(std::forward<Ts>(ts)...);
            throw;
        }
    }
}  // namespace igi

namespace igi::impl {
    struct empty {
        constexpr empty() { }

        template <typename T>
        constexpr empty(T &&) { }

        template <typename T>
        operator T() = delete;

        template <typename T>
        constexpr empty &operator=(T &&) { return *this; };
    };

    struct debug_ward {
        template <typename TTag, typename TMemberTag>
        static constexpr bool CompareTag() {
            return TTag::get() == TMemberTag::value;
        }

        template <typename TMemberTag, typename TClass>
        static constexpr decltype(auto) Get(TClass &&ins) {
            return ins.*(ins.template igi_ward<TMemberTag>());
        }
    };
}  // namespace igi::impl

#ifndef NDEBUG
#define igierror(...) (std::is_constant_evaluated() ? (void)0 : ::igi::LogError(__VA_ARGS__))

#define igiassert(cond, ...) (std::is_constant_evaluated() ? (void)0 : ::igi::Assert(static_cast<bool>((cond)), __FILE__, ':', __LINE__, ": ", #cond, " is false. ", ##__VA_ARGS__))

#define igiassert_eq(val, expect)                                             \
    do {                                                                      \
        decltype(auto) e = (expect);                                          \
        decltype(auto) v = (val);                                             \
        igiassert(v == e, #val, " is expected to be: ", e, ", but it's ", v); \
    } while (false)

#define igiward(name, ...)                                                                                                            \
    template <typename __TMemberTag, typename __TClass>                                                                               \
    friend constexpr decltype(auto)::igi::impl::debug_ward::Get(__TClass &&);                                                         \
    template <size_t>                                                                                                                 \
    struct igi_ward_tag;                                                                                                              \
    template <>                                                                                                                       \
    struct igi_ward_tag<__LINE__> {                                                                                                   \
        static constexpr std::string_view value = #name;                                                                              \
    };                                                                                                                                \
    template <typename __T>                                                                                                           \
    constexpr auto igi_ward(std::enable_if_t<::igi::impl::debug_ward::CompareTag<__T, igi_ward_tag<__LINE__>>()> * = nullptr) const { \
        return &std::decay_t<decltype(*this)>::name;                                                                                  \
    }                                                                                                                                 \
    std::enable_if_t<true, __VA_ARGS__> name

#define igiward_get(ins, name)                             \
    (([](decltype(ins) &__ins) -> const auto & {           \
        struct __tag {                                     \
            static constexpr decltype(auto) get() {        \
                return #name;                              \
            }                                              \
        };                                                 \
        return ::igi::impl::debug_ward::Get<__tag>(__ins); \
    })(ins))

#define igiward_set(ins, name, ...)                            \
    do {                                                       \
        (([](decltype(ins) &__ins) -> auto & {                 \
            struct __tag {                                     \
                static constexpr decltype(auto) get() {        \
                    return #name;                              \
                }                                              \
            };                                                 \
            return ::igi::impl::debug_ward::Get<__tag>(__ins); \
        })(ins))                                               \
            = (__VA_ARGS__);                                   \
    } while (false)
#else
#define igierror(...)

#define igiassert(cond, ...)

#define igiassert_eq(val, expect)

#define igiward(name, ...)

#define igiward_get(ins, name)

#define igiward_set(ins, name, ...)
#endif
