#pragma once

#include <iostream>

namespace igi {
    template <typename... Ts>
    inline void Assert(bool b, Ts &&... ts) {
        if (b)
            return;

        (std::cerr << ... << ts);
        if constexpr (sizeof...(Ts) > 0)
            std::cerr << std::endl;
        throw;
    }

}  // namespace igi

#if _DEBUG
#define igiassert(cond, ...) ::igi::Assert((cond), __FILE__, ' ', __LINE__, ": ", #cond, " is false. ", __VA_ARGS__)
#else
#define igiassert(cond, ...)
#endif