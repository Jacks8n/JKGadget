#pragma once

#include "igiscene/scene.h"

namespace igi {
    struct integrator_context {
        using itr_stack_t = typename aggregate::itr_stack_t;

        pcg32 pcg;
        itr_stack_t itrtmp;
        void *data;

        integrator_context(void *data = nullptr)
            : pcg(), itrtmp(context::GetTypedAllocator<itr_stack_t::value_type>()), data(data) { }
    };

    struct IIntegrator {
        META_BE_RT(IIntegrator)

        virtual color3 integrate(const scene &scene, ray &r, integrator_context &context) const = 0;
    };
}  // namespace igi
