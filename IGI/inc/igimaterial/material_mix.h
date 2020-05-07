#include "igimaterial/IMaterial.h"

namespace igi {
    class material_mix : public IMaterial {
        const IMaterial &_mat0;
        const IMaterial &_mat1;

        single _ratio;

      public:
        material_mix(const IMaterial &mat0, const IMaterial &mat1, single ratio)
            : _mat0(mat0), _mat1(mat1), _ratio(ratio) { }

        color_rgb operator()(const vec3f &i, const vec3f &o, const vec3f &n) const override {
            return Lerp(_mat0(i, o, n), _mat1(i, o, n), _ratio);
        }

        color_rgb getLuminance() const override {
            return Lerp(_mat0.getLuminance(), _mat1.getLuminance(), _ratio);
        }

        scatter getScatter(const vec3f &i, const mat3x3f& tanCoord, random_engine_t &rand) const override {
            return _ratio < std::uniform_real_distribution<single>()(rand)
                       ? _mat0.getScatter(i, tanCoord, rand)
                       : _mat1.getScatter(i, tanCoord, rand);
        }
    };
}  // namespace igi
