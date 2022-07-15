#ifndef _H_TYPES_H_

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

#undef TRUE
#undef FALSE
#define TRUE     (1)
#define FALSE    (0)

#define _H_TYPES_H_
#endif


