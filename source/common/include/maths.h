#ifndef H_MATHS_H

#include <math.h>

#include "types.h"

#ifndef PI
    #define PI             (3.14159265358979323846f)
#endif

#ifndef DEG_2_RAD
    #define DEG_2_RAD      (PI / 180.0f)
#endif

#ifndef RAD_2_DEG
    #define RAD_2_DEG      (180.0f / PI)
#endif

static inline f32 sin_f32(f32 deg)
{
    f32 result = sinf(deg);

    return result;
}

static inline f64 sin_f64(f64 deg)
{
    f64 result = sin(deg);

    return result;
}

static inline f32 cos_f32(f32 deg)
{
    f32 result = cosf(deg);

    return result;
}

static inline f64 cos_f64(f64 deg)
{
    f64 result = cos(deg);

    return result;
}

static inline f32 asin_f32(f32 value)
{
    f32 result = asinf(value);

    return result;
}

static inline f64 asin_f64(f64 value)
{
    f64 result = asin(value);

    return result;
}

static inline f32 acos_f32(f32 value)
{
    f32 result = acosf(value);

    return result;
}

static inline f64 acos_f64(f64 value)
{
    f64 result = acos(value);

    return result;
}

static inline f32 ceil_f32(f32 value)
{
    f32 result = ceilf(value);

    return result;
}

static inline f64 ceil_f64(f64 value)
{
    f64 result = ceil(value);

    return result;
}

static inline f32 floor_f32(f32 value)
{
    f32 result = floorf(value);

    return result;
}

static inline f64 floor_f64(f64 value)
{
    f64 result = floor(value);

    return result;
}

static inline f32 pow_f32(f32 value, f32 power)
{
    f32 result = powf(value, power);

    return result;
}

static inline f64 pow_f64(f64 value, f64 power)
{
    f64 result = pow(value, power);

    return result;
}

static inline f32 lerp(f32 min, f32 value, f32 max)
{
    f32 result = min + value * (max - min);

    return result;
}

static inline f32 clamp(f32 min, f32 value, f32 max)
{
    f32 result = value < min ? min : value;

    if (value > max)
    {
        result = max;
    }

    return result;
}

static inline f32 normalize(f32 min, f32 value, f32 max)
{
    f32 result = (value - min) / (max - min);

    return result;
}

// NOTE: There is also RGB macro defined in Windows.h
#undef RGB
#undef RGBA

// NOTE: Dangerous or not really?
#define V2(_x, _y)            (V2)  { .x = (_x), .y = (_y) }
#define V3(_x, _y, _z)        (V3)  { .x = (_x), .y = (_y), .z = (_z) }
#define V4(_x, _y, _z, _w)    (V4)  { .x = (_x), .y = (_y), .z = (_z), .w = (_w) }
#define RGBA(_r, _g, _b, _a)  (RGBA) { .r = (_r), .g = (_g), .b = (_b), .a = (_a) }
#define RGB(_r, _g, _b)       (RGB) { .r = (_r), .g = (_g), .b = (_b) }

#define H_MATHS_H
#endif
