#include "types.h"
#include "os_memory.h"

// NOTE: win32_memory functions must be matched with these types.
typedef void* (*OSAllocateMemory)(u64 size);
typedef void (*OSReleaseMemory)(void* memory);

typedef struct OSMemory
{
    OSAllocateMemory allocate;
    OSReleaseMemory release;
} OSMemory;

#ifdef WIN32
#include "win32_memory.h"

static OSMemory os_memory =
{
    .allocate = win32_allocate_memory,
    .release = win32_release_memory,
};

#else
#error WIN32 must be defined.
#endif

void* os_allocate_memory(u64 size)
{
    void* result = os_memory.allocate(size);

    return result;
}

void os_release_memory(void* memory)
{
    os_memory.release(memory);
}
