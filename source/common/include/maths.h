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

// NOTE: Dangerous or not really?
#define Vec2(_x, _y)          (Vec2)  { .x = (_x), .y = (_y) }
#define Vec3(_x, _y, _z)      (Vec3)  { .x = (_x), .y = (_y), .z = (_z) }
#define Vec4(_x, _y, _z, _w)  (Vec4)  { .x = (_x), .y = (_y), .z = (_z), .w = (_w) }
#define Color(_r, _g, _b, _a) (Color) { .r = (_r), .g = (_g), .b = (_b), .a = (_a) }
#define RGBA(_r, _g, _b, _a)  (Color) { .r = (_r), .g = (_g), .b = (_b), .a = (_a) }

#define H_MATHS_H
#endif
