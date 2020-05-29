#pragma once

#include "igimath/mathutil.h"
#include "rflite/rflite.h"

namespace igi {
    // type of color channel
    using col_c_t = float;

    constexpr col_c_t operator""_col(unsigned long long val) {
        return static_cast<col_c_t>(val);
    }

    constexpr col_c_t operator""_col(long double val) {
        return static_cast<col_c_t>(val);
    }

    class color3 {
      public:
        META_B(color3)

        META(r)
        col_c_t r;

        META(g)
        col_c_t g;

        META(b)
        col_c_t b;

        META_E

        constexpr color3(col_c_t r = 0_col, col_c_t g = 0_col, col_c_t b = 0_col) : r(r), g(g), b(b) { }

        static constexpr color3 Grey(col_c_t g) {
            return color3(g, g, g);
        }

        constexpr color3 saturated() const {
            return color3(Saturate(r), Saturate(g), Saturate(b));
        }

        constexpr color3 clamp(col_c_t lo, col_c_t hi) const {
            return color3(Clamp(lo, hi, r), Clamp(lo, hi, g), Clamp(lo, hi, b));
        }

        constexpr color3 clamp(const color3 &lo, const color3 &hi) const {
            return color3(Clamp(lo.r, hi.r, r), Clamp(lo.g, hi.g, g), Clamp(lo.b, hi.b, b));
        }

        constexpr col_c_t brightness() const {
            return .299_col * r + .587_col * g + .114_col * b;
        }

        constexpr color3 base() const {
            return operator*(1_col / brightness());
        }

        constexpr color3 toneMap(col_c_t adaptLum) const {
            constexpr col_c_t A = 2.51_col;
            constexpr col_c_t B = 0.03_col;
            constexpr col_c_t C = 2.43_col;
            constexpr col_c_t D = 0.59_col;
            constexpr col_c_t E = 0.14_col;

            color3 col = operator*(adaptLum);
            return (col * (col * A + B)) / (col * (col * C + D) + E);
        }

        constexpr color3 operator+(const color3 &c) const {
            return color3(r + c.r, g + c.g, b + c.b);
        }

        constexpr color3 operator-(const color3 &c) const {
            return color3(r - c.r, g - c.g, b - c.b);
        }

        constexpr color3 operator*(const color3 &c) const {
            return color3(r * c.r, g * c.g, b * c.b);
        }

        constexpr color3 operator/(const color3 &c) const {
            return color3(r / c.r, g / c.g, b / c.b);
        }

        constexpr color3 operator+(col_c_t s) const {
            return color3(r + s, g + s, b + s);
        }

        constexpr color3 operator-(col_c_t s) const {
            return color3(r - s, g - s, b - s);
        }

        constexpr color3 operator*(col_c_t s) const {
            return color3(r * s, g * s, b * s);
        }

        constexpr color3 operator/(col_c_t s) const {
            return color3(r / s, g / s, b / s);
        }

        col_c_t &operator[](size_t index) {
            assert(index < 3);
            switch (index) {
                case 0: return r;
                case 1: return g;
                default: return b;
            }
        }

        constexpr col_c_t operator[](size_t index) const {
            assert(index < 3);
            switch (index) {
                case 0: return r;
                case 1: return g;
                default: return b;
            }
        }
    };

    struct palette {
        static constexpr color3 white = color3::Grey(1_col);

        static constexpr color3 black = color3::Grey(0_col);

        static constexpr color3 red = color3(1_col, 0_col, 0_col);

        static constexpr color3 green = color3(0_col, 1_col, 0_col);

        static constexpr color3 blue = color3(0_col, 0_col, 1_col);

        static constexpr color3 yellow = color3(1_col, 1_col, 0_col);
    };
}  // namespace igi
