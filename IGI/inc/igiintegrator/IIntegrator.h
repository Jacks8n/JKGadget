#pragma once

#include "igiscene/scene.h"

namespace igi {
    struct integrator_context {
        using itr_stack_t = typename aggregate::itr_stack_t;

        pcg32 pcg;

        itr_stack_t itrtmp;

        integrator_context()
            : pcg(GetSeed()), itrtmp(context::GetTypedAllocator<typename itr_stack_t::value_type>()) {
        }

      private:
        static uint64_t GetSeed() {
#ifdef NDEBUG
            static std::random_device rd;
            return rd();
#else
            return 0;
#endif
        }
    };

    struct IIntegrator {
        META_BE_RT(IIntegrator)

        virtual color3 integrate(const scene &scene, ray &r, integrator_context &context) const = 0;
    };
}  // namespace igi
