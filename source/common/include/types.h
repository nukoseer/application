#ifndef H_TYPES_H

#include <stdint.h>

typedef int8_t     i8;
typedef int16_t    i16;
typedef int32_t    i32;
typedef int64_t    i64;
                   
typedef uint8_t    u8;
typedef uint16_t   u16;
typedef uint32_t   u32;
typedef uint64_t   u64;
                   
typedef int32_t    b32; // NOTE: For bool.
                   
typedef float      f32;
typedef double     f64;
                   
typedef size_t     memory_size;
typedef uintptr_t  uptr;

// NOTE: /wd4201
typedef union V2
{
    struct { f32 x, y; };
    f32 e[2];
} V2;

typedef union V3
{
    struct { f32 x, y, z; };
    struct { f32 r, g, b; };
    f32 e[3];
} V3, RGB;

typedef union V4
{
    struct { f32 x, y, z, w; };
    struct { f32 r, g, b, a; };
    f32 e[4];
} V4, RGBA;

#undef TRUE
#undef FALSE
#define TRUE      (1)
#define FALSE     (0)

#define H_TYPES_H
#endif


