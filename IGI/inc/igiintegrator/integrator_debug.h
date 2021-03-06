﻿#pragma once

#include "igiintegrator/IIntegrator.h"

namespace igi {
    enum class integrator_debug_mode : size_t { postion      = 0,
                                                normal       = 1,
                                                uv           = 2,
                                                dpdu         = 3,
                                                dpdv         = 4,
                                                depth        = 5,
                                                material_lum = 6 };

    class integrator_debug : public IIntegrator {
        using debug_func_t = color3 (*)(ray &r, interaction &i);

        debug_func_t _debug;

      public:
        META_BE_RT(integrator_debug, ser_pmr_name_a("debug"), deser_pmr_func_a<IIntegrator>([](const serializer_t &ser) {
                       auto mode = static_cast<integrator_debug_mode>(serialization::Deserialize<size_t>(ser["mode"]));

                       IIntegrator *debug = context::New<integrator_debug>(mode);
                       return debug;
                   }))

        integrator_debug(integrator_debug_mode mode) {
            debug(mode);
        }

        void debug(integrator_debug_mode mode) {
            switch (mode) {
                case integrator_debug_mode::postion:
                    _debug = debugpostion;
                    break;
                case integrator_debug_mode::normal:
                    _debug = debugnormal;
                    break;
                case integrator_debug_mode::uv:
                    _debug = debuguv;
                    break;
                case integrator_debug_mode::dpdu:
                    _debug = debugdpdu;
                    break;
                case integrator_debug_mode::dpdv:
                    _debug = debugdpdv;
                    break;
                case integrator_debug_mode::depth:
                    _debug = debugdepth;
                    break;
                case integrator_debug_mode::material_lum:
                    _debug = debugmaterial_lum;
                    break;
                default:
                    igierror("mode ", static_cast<size_t>(mode), " not supported");
                    break;
            }
        }

        color3 integrate(const scene &scene, ray &r, integrator_context &context) const override {
            const aggregate &agg = scene.getAggregate();

            interaction iact;
            if (!agg.tryHit(r, &iact, context.itrtmp))
                return palette::black;

            return _debug(r, iact);
        }

      private:
        static constexpr color3 vecToCol(const vec2f &v) {
            return color3(Abs(v[0]), Abs(v[1]), 0_col);
        }

        static constexpr color3 vecToCol(const vec3f &v) {
            return color3(Abs(v[0]), Abs(v[1]), Abs(v[2]));
        }

        static color3 debugpostion(ray &r, interaction &i) {
            return vecToCol(i.surface.position);
        }

        static color3 debugnormal(ray &r, interaction &i) {
            return vecToCol(i.surface.normal);
        }

        static color3 debuguv(ray &r, interaction &i) {
            return vecToCol(i.surface.uv);
        }

        static color3 debugdpdu(ray &r, interaction &i) {
            return vecToCol(i.surface.dpdu);
        }

        static color3 debugdpdv(ray &r, interaction &i) {
            return vecToCol(i.surface.dpdv);
        }

        static color3 debugdepth(ray &r, interaction &i) {
            return color3::Grey(std::exp(-r.getT()));
        }

        static color3 debugmaterial_lum(ray &r, interaction &i) {
            return i.material->getLuminance();
        }
    };
}  // namespace igi
