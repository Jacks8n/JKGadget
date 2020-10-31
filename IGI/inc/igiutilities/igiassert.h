#pragma once

#include <iostream>

namespace igi {
    template <typename... Ts>
    inline void igiassert(bool b, Ts &&... ts) {
        if (b)
            return;

        (std::cerr << ... << ts);
        if constexpr (sizeof...(Ts))
            std::cerr << std::endl;
        throw;
    }
}  // namespace igi