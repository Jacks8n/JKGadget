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

#if _DEBUG
#define igierror(...) (std::is_constant_evaluated() ? (void)0 : ::igi::LogError(__VA_ARGS__))

#define igiassert(cond, ...) (std::is_constant_evaluated() ? (void)0 : ::igi::Assert((cond), __FILE__, ':', __LINE__, ": ", #cond, " is false. ", ##__VA_ARGS__))

#define igiassert_eq(val, expect)                                             \
    do {                                                                      \
        decltype(auto) e = (expect);                                          \
        decltype(auto) v = (val);                                             \
        igiassert(v == e, #val, " is expected to be: ", e, ", but it's ", v); \
    } while (false)

#define igidebug_begin()                   \
    if (!::std::is_constant_evaluated()) { \
        do {
#define igidebug_end() \
    }                  \
    while (false)      \
        ;              \
    }
#else
#define igierror(...)

#define igiassert(cond, ...)

#define igidebug_begin() if constexpr (false) {
#define igidebug_end()   }
#endif