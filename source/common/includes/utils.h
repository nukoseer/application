#ifndef H_UTILS_H

#include <types.h>
#include "os_memory.h"

#define ASSERT(x) do { if (!(x)) { *(volatile int*)0; } } while (0)

#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define OFFSETOF(T, m) ((size_t)(&(((T*)0)->m)))

#define KILOBYTES(n) ((n) << 10)
#define MEGABYTES(n) (KILOBYTES(n) << 10)
#define GIGABYTES(n) (MEGABYTES(n) << 10)

// NOTE: Stretchy buffer implementation.
typedef struct BufferHeader
{
    memory_size len;
    memory_size cap;
    u8 buf[]; // NOTE: MSVC supports flexible array members as a compiler extension.
} BufferHeader;

// NOTE: Internal macros.
#define BUFF__BASE(buff) ((BufferHeader*)(void*)((u8*)(buff) - OFFSETOF(BufferHeader, buf)))
#define BUFF__LENGTH(buff) ((buff) ? BUFF__BASE(buff)->len : 0)
#define BUFF__CAPACITY(buff) ((buff) ? BUFF__BASE(buff)->cap : 0)
#define BUFF__SHOULD_GROW(buff, n) (BUFF__LENGTH(buff) + (n) <= BUFF__CAPACITY(buff))
#define BUFF__GROW(buff, n) (BUFF__SHOULD_GROW(buff, n) ? 0 : ((buff) = buffer_grow(buff, (n) + BUFF__LENGTH(buff), sizeof(*(buff)))))

// NOTE: User macros.
#define BUFFER_LENGTH(buff) BUFF__LENGTH(buff)
#define BUFFER_CAPACITY(buff) BUFF__CAPACITY(buff)
#define BUFFER_PUSH(buff, x) (BUFF__GROW(buff, 1), (buff)[BUFF__BASE(buff)->len++] = (x))
#define BUFFER_FREE(buff) ((buff) ? (os_memory_heap_release(BUFF__BASE(buff)), (buff) = NULL) : 0)

inline void* buffer_grow(void* buffer, memory_size new_capacity, memory_size element_size)
{
    memory_size capacity = MAX(1 + BUFFER_CAPACITY(buffer) * 2, new_capacity);
    memory_size size = capacity * element_size + OFFSETOF(BufferHeader, buf);
    BufferHeader* buffer_header = NULL;

    ASSERT(capacity >= new_capacity);

    if (!buffer)
    {
        buffer_header = (BufferHeader*)os_memory_heap_allocate(size);
        buffer_header->len = 0;
    }
    else
    {
        buffer_header = (BufferHeader*)os_memory_heap_reallocate(BUFF__BASE(buffer), size);
    }

    buffer_header->cap = capacity;

    return buffer_header->buf;
}
// NOTE: End of stretcy buffer implementation.

#define H_UTILS_H
#endif
