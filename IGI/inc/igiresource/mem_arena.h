#pragma once

#include <memory_resource>

namespace igi {
    class mem_arena : std::pmr::memory_resource {
        protected:
        void foo() {
            std::pmr::memory_resource::
        }
    };
}  // namespace igi