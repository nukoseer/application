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

// NOTE: /wd4201
typedef union Vec2
{
    struct { f32 x, y; };
    f32 e[2];
} Vec2;

typedef union Vec3
{
    struct { f32 x, y, z; };
    struct { f32 r, g, b; };
    f32 e[3];
} Vec3;

typedef union Vec4
{
    struct { f32 x, y, z, w; };
    struct { f32 r, g, b, a; };
    f32 e[4];
} Vec4;

f32 sin_f32(f32 deg)
{
    f32 result = sinf(deg);

    return result;
}

f64 sin_f64(f64 deg)
{
    f64 result = sin(deg);

    return result;
}

f32 cos_f32(f32 deg)
{
    f32 result = cosf(deg);

    return result;
}

f64 cos_f64(f64 deg)
{
    f64 result = cos(deg);

    return result;
}


#define H_MATHS_H
#endif
