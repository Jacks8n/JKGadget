#pragma once

#include "numconfig.h"

namespace igi {
    constexpr single Pi = 3.1415926535897932384626433832795;

    constexpr single PiHalf = 1.5707963267948966192313216916398;

    constexpr single PiQuarter = 0.78539816339744830961566084581988;

    constexpr single PiTwo = 6.283185307179586476925286766559;

    constexpr single PiFour = 12.566370614359172953850573533118;

    constexpr single Deg2Rad = 0.01745329251994329576923690768489;

    constexpr single PiInv = 0.31830988618379067153776752674503;

    constexpr single PiTwoInv = 0.15915494309189533576888376337251;

    constexpr single ToRad(single deg) {
        return deg * Deg2Rad;
    }

    constexpr single PiTwoToZeroOne(single val) {
        return val * PiTwoInv + .5_sg;
    }

    constexpr single ZeroOneToPiTwo(single val) {
        return val * PiTwo - Pi;
    }
}  // namespace igi
