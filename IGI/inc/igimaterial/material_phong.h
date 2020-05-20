#include "igimaterial/IMaterial.h"

namespace igi {
    class material_phong : public IMaterial {
        single _shin;

        color3 _spec;

      public:
        material_phong(single shin = 5_sg, color3 spec = palette::white)
            : _shin(shin), _spec(spec) { }

        color3 operator()(const vec3f &i, const vec3f &o, const vec3f &n) const override {
            single d = Dot(Reflect(i, n), o);
            if (IsPoscf(d))
                return _spec * pow(d, _shin);
            return 0_sg;
        }

        color3 getLuminance() const override {
            return palette::black;
        }

        scatter getScatter(const vec3f &i, const mat3x3f &normalSpace, random_engine_t &rand) const override {
            hemisphere_cos_distribution hcd;
            scatter s;
            s.direction = normalSpace * hcd(rand, &s.pdf);
            return s;
        }
    };
}  // namespace igi
