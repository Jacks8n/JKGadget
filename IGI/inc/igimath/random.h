#pragma once

#include <random>
#include "igimath/const.h"
#include "igimath/vec.h"

namespace igi {
    class pcg32 {
        uint64_t _state;

        uint64_t _inc;

      public:
        using result_type = uint32_t;

        pcg32(uint64_t state = 0, uint64_t inc = 0) : _state(state), _inc(inc | 1) { }

        void seed(uint64_t state, uint64_t inc = 0) {
            _state = state;
            _inc   = inc | 1;
        }

        uint32_t min() const { return 0u; }

        uint32_t max() const { return ~0u; }

        void discard(unsigned long long u) {
            while (u) {
                operator()();
                --u;
            }
        }

        uint32_t operator()() {
            uint64_t oldstate   = _state;
            _state              = oldstate * 6364136223846793005ULL + _inc;
            uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
            uint32_t rot        = oldstate >> 59u;
            return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
        }
    };

    class unit_square_distribution {
        std::uniform_real_distribution<single> _urd;

      public:
        template <typename Te>
        vec2f operator()(Te &&engine) const {
            single p;
            return operator()(engine, &p);
        }

        template <typename Te>
        vec2f operator()(Te &&engine, single *p) const {
            *p = 1;

            return vec2<single>(_urd(engine), _urd(engine));
        }
    };

    class unit_disk_distribution {
        std::uniform_real_distribution<single> _urd;

      public:
        template <typename Te>
        vec2f operator()(Te &&engine) const {
            single p;
            return operator()(engine, &p);
        }

        template <typename Te>
        vec2f operator()(Te &&engine, single *p) const {
            single r0    = _urd(engine);
            single r1    = _urd(engine);
            single rho   = sqrt(r0);
            single theta = PiTwo * r1;

            *p = r0;
            return vec2<single>(rho * cos(theta), rho * sin(theta));
        }
    };

    class hemisphere_cos_distribution {
        unit_disk_distribution _udd;

      public:
        template <typename Te>
        vec3f operator()(Te &&engine) const {
            single p;
            return operator()(engine, &p);
        }

        template <typename Te>
        vec3f operator()(Te &&engine, single *p) const {
            vec2f v = _udd(engine, p);
            return vec3f(v, sqrt(Saturate(1 - v.magnitudeSqr())));
        }
    };
}  // namespace igi
