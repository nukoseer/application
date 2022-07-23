#include "utils.h"
#include "types.h"
#include "os_memory.h"

typedef void* OSAllocateMemory(u64 size);
typedef void  OSReleaseMemory(void* memory);

typedef struct OSMemory
{
    OSAllocateMemory* allocate;
    OSReleaseMemory* release;
} OSMemory;

#ifdef WIN32
#include "win32_memory.h"

static OSMemory os_memory =
{
    .allocate = &win32_memory_allocate,
    .release = &win32_memory_release,
};

#else
#error WIN32 must be defined.
#endif

void* os_memory_allocate(u64 size)
{
    void* result;
    
    ASSERT(os_memory.allocate);
    result = os_memory.allocate(size);

    return result;
}

void os_memory_release(void* memory)
{
    ASSERT(os_memory.release);
    os_memory.release(memory);
}
