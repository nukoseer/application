#ifndef H_UTILS_H

#include <types.h>
#include <stdlib.h>

#define ASSERT(x) do { if (!(x)) { *(volatile int*)0; } } while (0)

#define MAX(a, b) ((a) >= (b) ? (a) : (b))

// TODO: Probably stretchy buffer needs its own file.

// NOTE: Internal macros.
#define BUFF__DEFAULT_ELEMENT_TYPE u32
#define BUFF__DEFAULT_ELEMENT_SIZE (sizeof(BUFF__DEFAULT_ELEMENT_TYPE))
#define BUFF__BASE(buff) ((BUFF__DEFAULT_ELEMENT_TYPE*)(buff) - 2)
#define BUFF__LENGTH(buff) ((buff) ? *BUFF__BASE(buff) : 0)
#define BUFF__CAPACITY(buff) ((buff) ? *(BUFF__BASE(buff) + 1) : 0)
#define BUFF__SHOULD_GROW(buff, n) (BUFFER_LENGTH(buff) + (n) <= BUFFER_CAPACITY(buff))
#define BUFF__GROW(buff, n) (BUFF__SHOULD_GROW(buff, n) ? 0 : ((buff) = buffer_grow(buff, n + BUFF__LENGTH(buff), sizeof(*(buff)))))

// NOTE: User macros.
#define BUFFER_LENGTH(buff) BUFF__LENGTH(buff)
#define BUFFER_CAPACITY(buff) BUFF__CAPACITY(buff)
#define BUFFER_PUSH(buff, x) (BUFF__GROW(buff, 1), (buff)[BUFFER_LENGTH(buff)] = (x), *BUFF__BASE(buff) = *BUFF__BASE(buff) + 1)
#define BUFFER_FREE(buff) (buff && BUFF__BASE(buff) ? free(BUFF__BASE(buff)) : (void)0)

inline void* buffer_grow(void* buffer, u32 new_capacity, u32 element_size)
{
    u32 capacity = MAX(new_capacity, BUFF__CAPACITY(buffer) * 2);
    BUFF__DEFAULT_ELEMENT_TYPE* buff = 0;
    
    if (!buffer)
    {
        buff = (BUFF__DEFAULT_ELEMENT_TYPE*)malloc(capacity * element_size + 2 * BUFF__DEFAULT_ELEMENT_SIZE);
        *buff = 0; // NOTE: Length
    }
    else
    {
        buffer = BUFF__BASE(buffer);
        buff = (BUFF__DEFAULT_ELEMENT_TYPE*)realloc(buffer, capacity * element_size + 2 * BUFF__DEFAULT_ELEMENT_SIZE);
    }

    buff++;
    *buff++ = capacity; // NOTE: Capacity

    return (void*)buff;
}

#define H_UTILS_H
#endif

