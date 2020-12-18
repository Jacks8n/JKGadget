#include "igimaterial/IMaterial.h"

namespace igi {
    class material_phong : public IMaterial {
        single _shin;

        color3 _spec;

      public:
        META_BE_RT(material_phong, ser_pmr_name_a("phong"), deser_pmr_func_a<IMaterial>([](const serializer_t &ser) {
                       IGI_SERIALIZE_OPTIONAL(single, shine, 5_sg, ser);
                       IGI_SERIALIZE_OPTIONAL(color3, specular, palette::white, ser);

                       IMaterial *p = context::New<material_phong>(shine, specular);
                       return p;
                   }))

        material_phong(single shin = 5_sg, color3 spec = palette::white)
            : _shin(shin), _spec(spec) { }

        color3 operator()(const vec3f &i, const vec3f &o, const vec3f &n) const override {
            single d = Dot(Reflect(i, n), o);
            if (d > 0_sg)
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
