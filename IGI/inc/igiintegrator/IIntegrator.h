#pragma once

#include "igiscene/scene.h"

namespace igi {
    struct integrator_context {
        using itr_stack_t    = typename aggregate::itr_stack_t;
        using allocator_type = std::pmr::polymorphic_allocator<typename itr_stack_t::value_type>;

        pcg32 pcg;
        itr_stack_t itrtmp;

        integrator_context(const allocator_type &alloc)
            : pcg(), itrtmp(alloc) { }
    };

    struct IIntegrator {
        virtual color3 integrate(const scene &scene, ray &r, integrator_context &context) const = 0;
    };
}  // namespace igi
