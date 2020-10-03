#pragma once

#include "igimath/single.h"

namespace igi {
    constexpr single Pi = 3.1415926535897932384626433832795_sg;

    constexpr single PiHalf = 1.5707963267948966192313216916398_sg;

    constexpr single PiQuarter = 0.78539816339744830961566084581988_sg;

    constexpr single PiTwo = 6.283185307179586476925286766559_sg;

    constexpr single PiFour = 12.566370614359172953850573533118_sg;

    constexpr single Deg2Rad = 0.01745329251994329576923690768489_sg;

    constexpr single PiInv = 0.31830988618379067153776752674503_sg;

    constexpr single PiTwoInv = 0.15915494309189533576888376337251_sg;

    constexpr auto ToRad(const single &deg) {
        return deg * Deg2Rad;
    }

    constexpr auto PiTwoToZeroOne(const single &val) {
        return val * PiTwoInv + .5_sg;
    }

    constexpr auto ZeroOneToPiTwo(const single &val) {
        return val * PiTwo - Pi;
    }
}  // namespace igi
