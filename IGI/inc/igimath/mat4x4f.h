#pragma once

#include <immintrin.h>
#include "igimath/vec.h"

namespace igi {
    template <>
    class alignas(32) matrix<float, 4, 4> : public matrix_base_sqr<float, 4> {
      public:
        using matrix_base_sqr<float, 4>::matrix_base_sqr;

        matrix() = default;

        vec3f mulPos(const vec3f &p) const {
            vec<float, 4> vp(p, 1_sg);
            vp = operator*(vp);
            return vec3f(vp) * vp[3];
        }

        vec3f mulVec(const vec3f &v) const {
            vec<float, 4> vp(v, 0_sg);
            return vec3f(operator*(vp));
        }

        vec4f operator*(const vec<float, 4> &r) const {
            alignas(32) float res[8];
            for (size_t i = 0; i < 4; i++) {
                res[i]     = r[i];
                res[i + 4] = r[i];
            }

            __m256 row01 = _mm256_load_ps(&get(0, 0));
            __m256 row23 = _mm256_load_ps(&get(2, 0));

            __m256 col = _mm256_load_ps(res);

            __m256 res_xy = _mm256_dp_ps(row01, col, 0xFF);
            __m256 res_zw = _mm256_dp_ps(row23, col, 0xFF);

            __m256 res_xzyw = _mm256_blend_ps(res_xy, res_zw, 0b11001100);

            __m256 res_ywxz = _mm256_permute2f128_ps(res_xzyw, res_xzyw, 1);

            __m256 res_xyzw = _mm256_blend_ps(res_xzyw, res_ywxz, 0b01011010);

            _mm256_store_ps(res, res_xyzw);
            return vec4f(res[0], res[1], res[2], res[3]);
        }

        matrix operator*(const matrix &r) const {
            matrix res;

            __m256 row01 = _mm256_load_ps(&get(0, 0));
            __m256 row23 = _mm256_load_ps(&get(2, 0));

            // m11, m12, m13, m14, m21, m22, m23, m24
            __m256 rrow01 = _mm256_load_ps(&r.get(0, 0));
            // m31, m32, m33, m34, m41, m42, m43, m44
            __m256 rrow23 = _mm256_load_ps(&r.get(2, 0));

            // m11, m12, m31, m32, m21, m22, m41, m42
            __m256 rcol01_xzyw = _mm256_shuffle_ps(rrow01, rrow23, 0b01000100);
            // m13, m14, m33, m34, m23, m24, m43, m44
            __m256 rcol23_xzyw = _mm256_shuffle_ps(rrow01, rrow23, 0b11101110);

            // m21, m22, m41, m42, m11, m12, m31, m32
            __m256 rcol01_ywxz = _mm256_permute2f128_ps(rcol01_xzyw, rcol01_xzyw, 1);
            // m23, m24, m43, m44, m13, m14, m33, m34
            __m256 rcol23_ywxz = _mm256_permute2f128_ps(rcol23_xzyw, rcol23_xzyw, 1);

            // m22, m21, m42, m41, m12, m11, m32, m31
            __m256 rcol10_ywxz = _mm256_shuffle_ps(rcol01_ywxz, rcol01_ywxz, 0b10110001);
            // m24, m23, m44, m43, m14, m13, m34, m33
            __m256 rcol32_ywxz = _mm256_shuffle_ps(rcol23_ywxz, rcol23_ywxz, 0b10110001);

            // m11, m21, m31, m41, m12, m22, m32, m42
            __m256 rcol01 = _mm256_blend_ps(rcol01_xzyw, rcol10_ywxz, 0b01011010);
            // m13, m23, m33, m43, m14, m24, m34, m44
            __m256 rcol23 = _mm256_blend_ps(rcol23_xzyw, rcol32_ywxz, 0b01011010);

            // m11, m11, m11, m11, m22, m22, m22, m22
            __m256 res01_1122 = _mm256_dp_ps(row01, rcol01, 0xFF);
            // m13, m13, m13, m13, m24, m24, m24, m24
            __m256 res01_1324 = _mm256_dp_ps(row01, rcol23, 0xFF);
            // m11, m11, m13, m13, m22, m22, m24, m24
            __m256 res01_xz = _mm256_blend_ps(res01_1122, res01_1324, 0b11001100);

            // m31, m31, m31, m31, m42, m42, m42, m42
            __m256 res23_3142 = _mm256_dp_ps(row23, rcol01, 0xFF);
            // m33, m33, m33, m33, m44, m44, m44, m44
            __m256 res23_3344 = _mm256_dp_ps(row23, rcol23, 0xFF);
            // m31, m31, m33, m33, m42, m42, m44, m44
            __m256 res23_xz = _mm256_blend_ps(res23_3142, res23_3344, 0b11001100);

            // m21, m22, m41, m42, m11, m12, m31, m32
            __m256 rcol10 = _mm256_permute2f128_ps(rcol01, rcol01, 1);
            // m14, m24, m34, m44, m13, m23, m33, m43
            __m256 rcol32 = _mm256_permute2f128_ps(rcol23, rcol23, 1);

            // m12, m12, m12, m12, m21, m21, m21, m21
            __m256 res01_1221 = _mm256_dp_ps(row01, rcol10, 0xFF);
            // m11, m12, m13, m13, m21, m22, m24, m24
            __m256 res01_xyz = _mm256_blend_ps(res01_xz, res01_1221, 0b00010010);

            // m14, m14, m14, m14, m23, m23, m23, m23
            __m256 res01_1423 = _mm256_dp_ps(row01, rcol32, 0xFF);
            // m11, m12, m13, m14, m21, m22, m23, m24
            __m256 res01 = _mm256_blend_ps(res01_xyz, res01_1423, 0b01001000);

            // m32, m32, m32, m32, m41, m41, m41, m41
            __m256 res23_3241 = _mm256_dp_ps(row23, rcol10, 0xFF);
            // m31, m32, m33, m33, m41, m42, m44, m44
            __m256 res23_xyz = _mm256_blend_ps(res23_xz, res23_3241, 0b00010010);

            // m34, m34, m34, m34, m43, m43, m43, m43
            __m256 res23_4334 = _mm256_dp_ps(row23, rcol32, 0xFF);
            // m31, m32, m33, m34, m41, m42, m43, m44
            __m256 res23 = _mm256_blend_ps(res23_xyz, res23_4334, 0b01001000);

            _mm256_store_ps(&res.get(0, 0), res01);
            _mm256_store_ps(&res.get(2, 0), res23);

            return res;
        }
    };
}  // namespace igi
