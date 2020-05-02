#include "igimaterial/IMaterial.h"

namespace igi {
    class material_phong : public IMaterial {
        single _shin;

        color_rgb _spec;

        hemisphere_cos_distribution _hcd;

      public:
        material_phong(single shin = 5, color_rgb spec = palette_rgb::white)
            : _shin(shin), _spec(spec) { }

        color_rgb operator()(const vec3f &i, const vec3f &o, const vec3f &n) const override {
            return _spec * pow(Dot(Reflect(i, n), o), _shin);
        }

        color_rgb getLuminance() const override {
            return palette_rgb::black;
        }

        scatter getScatter(const vec3f &i, const mat3x3f &tanCoord, pcg32 &rand) const override {
            scatter s;
            s.direction = tanCoord * _hcd(rand, &s.pdf);
            return s;
        }
    };
}  // namespace igi
