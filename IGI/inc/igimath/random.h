#pragma once

#include <random>
#include "igimath/vec.h"
#include "igimath/const.h"

namespace igi {
    class pcg32 {
        mutable uint64_t _state;

        uint64_t _inc;

    public:
        pcg32(uint64_t state = 0, uint64_t inc = 0) : _state(state), _inc(inc | 1) {}

        void reset(uint64_t stat = 0, uint64_t inc = 0) {
            _state = stat;
            _inc = inc;
        }

        uint32_t min() const { return 0u; }

        uint32_t max() const { return ~0u; }

        uint32_t operator()() const {
            uint64_t oldstate = _state;
            _state = oldstate * 6364136223846793005ULL + _inc;
            uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
            uint32_t rot = oldstate >> 59u;
            return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
        }
    };

    class unit_disk_distribution {
    public:
        template<typename Te>
        vec2f operator()(Te& engine) const {
            single p;
            return operator()(engine, p);
        }

        template<typename Te>
        vec2f operator()(Te& engine, single& p) const {
            std::uniform_real_distribution<single> r;

            single r0 = r(engine), r1 = r(engine);
            p = 2 * r0;
            single rho = sqrt(r0);
            single theta = PiTwo * r1;

            return vec2<single>(rho * cos(theta), rho * sin(theta));
        }
    };

    class hemisphere_cos_distribution {
    public:
        template<typename Te>
        vec3f operator()(Te& engine) const {
            single p;
            return operator()(engine, p);
        }

        template<typename Te>
        vec3f operator()(Te& engine, single &p) const {
            vec2f v = unit_disk_distribution()(engine, p);
            return vec3f(v, sqrt(saturate(1 - v.magnitudeSqr())));
        }
    };
}
