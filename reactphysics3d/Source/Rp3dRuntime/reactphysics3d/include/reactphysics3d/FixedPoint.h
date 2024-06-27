#ifndef FIXED_POINT_H
#define FIXED_POINT_H

#define RP3D_USE_FIXED

#ifdef RP3D_USE_FIXED

#define FIXED_64_ENABLE_TRIG_LUT 1
#define FIXED_64_ENABLE_SATURATING 1
#define FIXED_64_ENABLE_OVERFLOW 0
#define FIXED_64_FORCE_EVALUATE_IN_COMPILE_TIME 0
#define FIXED_64_ENABLE_INT128_ACCELERATION 0
#define FIXED_64_ENABLE_FORCEINLINE 1

#include "fixed64.hpp"

using FixedPoint = f64::fixed64<32>;


#define rp3dAbs(x) f64::abs(x)
#define rp3dSqrt(x) f64::sqrt(x)
#define rp3dPow(x,y) f64::pow(x,y)
#define rp3dCeil(x) f64::ceil(FixedPoint(x))
#define rp3dFloor(x) f64::floor(x)
#define rp3dRound(x) f64::round(x)
#define rp3dFmod(a,b) f64::fmod(a,b)


#define rp3dSin(x) f64::sin(x)
#define rp3dCos(x) f64::cos(x)

#define rp3dAsin(x) f64::asin(x)
#define rp3dAcos(x) f64::acos(x)
#define rp3dAtan(x) f64::atan(x)
#define rp3dAtan2(x,y) f64::atan2(x,y)

namespace std
{
    constexpr inline bool isfinite(FixedPoint x) noexcept
    {
#if FIXED_64_ENABLE_OVERFLOW && FIXED_64_ENABLE_SATURATING
        using limits = std::numeric_limits<FixedPoint>;
        return x != limits::max() && x != limits::min();
#else
        return true;
#endif
    }
}

#else

#define rp3dAbs(x) std::abs(x)
#define rp3dSqrt(x) std::sqrt(x)
#define rp3dPow(x,y) std::pow(x,y)
#define rp3dCeil(x) std::ceil(x)
#define rp3dFloor(x) std::floor(x)
#define rp3dRound(x) std::round(x)
#define rp3dFmod(a,b) std::fmod(a,b)


#define rp3dSin(x) std::sin(x)
#define rp3dCos(x) std::cos(x)

#define rp3dAsin(x) std::asin(x)
#define rp3dAcos(x) std::acos(x)
#define rp3dAtan(x) std::atan(x)
#define rp3dAtan2(x,y) std::atan2(x,y)

#endif

#endif

