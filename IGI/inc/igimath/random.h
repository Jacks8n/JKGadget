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

        static constexpr uint64_t DefaultSeed = 0;

        pcg32(uint64_t state = DefaultSeed, uint64_t inc = 0) : _state(state), _inc(inc | 1) { }

        void seed(uint64_t state = DefaultSeed) {
            _state = state;
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

    using random_engine_t = pcg32;

    class uniform_quad_distribution {
        std::uniform_real_distribution<single> _urdX, _urdY;

        single _prob;

      public:
        uniform_quad_distribution() : _urdX(), _urdY(), _prob(1_sg) { }

        uniform_quad_distribution(const vec2f &min, const vec2f &max)
            : _urdX(min[0], max[0]), _urdY(min[1], max[1]), _prob(1_sg / (max - min).l1norm()) {
        }

        template <typename Te>
        vec2f operator()(Te &&engine) {
            single p;
            return operator()(std::forward<Te>(engine), &p);
        }

        template <typename Te>
        vec2f operator()(Te &&engine, single *p) {
            *p = _prob;

            return vec2<single>(_urdX(engine), _urdY(engine));
        }
    };

    class unit_disk_distribution {
        std::uniform_real_distribution<single> _urd;

      public:
        template <typename Te>
        vec2f operator()(Te &&engine) {
            single p;
            return operator()(std::forward<Te>(engine), &p);
        }

        template <typename Te>
        vec2f operator()(Te &&engine, single *p) {
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
        vec3f operator()(Te &&engine) {
            single p;
            return operator()(std::forward<Te>(engine), &p);
        }

        template <typename Te>
        vec3f operator()(Te &&engine, single *p) {
            vec2f v = _udd(engine, p);
            return vec3f(v, sqrt(Saturate(1 - v.magnitudeSqr())));
        }
    };

    template <size_t Dim>
    class uniform_vector_distribution {
        std::uniform_real_distribution<single> _urd[Dim];

      public:
        uniform_vector_distribution(const vecf<Dim> &lo, const vecf<Dim> &hi) {
            for (size_t i = 0; i < Dim; i++)
                _urd[i] = std::uniform_real_distribution<single>(lo[i], hi[i]);
        }

        template <typename Te>
        vecf<Dim> operator()(Te &&engine) {
            single p;
            return operator()(std::forward<Te>(engine), &p);
        }

        template <typename Te>
        vecf<Dim> operator()(Te &&engine, single *p) {
            vecf<Dim> res;
            for (size_t i = 0; i < Dim; i++)
                res[i] = _urd[i](engine);
            *p = 1;
            return res;
        }
    };
}  // namespace igi
