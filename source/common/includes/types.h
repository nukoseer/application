#ifndef H_TYPES_H

#include <stdint.h>

typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int32_t  b32; // NOTE: For bool.

typedef float    f32;
typedef double   f64;

typedef size_t   memory_size;

#undef TRUE
#undef FALSE
#define TRUE     (1)
#define FALSE    (0)

#define H_TYPES_H
#endif


