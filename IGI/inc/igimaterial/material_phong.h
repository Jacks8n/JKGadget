#include "igimaterial/IMaterial.h"

namespace igi {
    class material_phong : public IMaterial {
    public:
        single shinness = 5;
        color_rgb specular = palette_rgb::white;

        color_rgb operator()(const vec3f& i, const vec3f& o, const vec3f& n) const override {
            return specular * pow(Dot(Reflect(i, n), o), shinness);
        }

        color_rgb getLuminance() const override {
            return palette_rgb::white;
        }

        scatter getScatter(const vec3f& i, const vec3f& n) const override {
            return scatter(Reflect(i, n), 1);
        }
    };
}
