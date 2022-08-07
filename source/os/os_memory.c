#include "utils.h"
#include "types.h"
#include "os_memory.h"

typedef void* OSMemoryReserve(memory_size size);
typedef void  OSMemoryCommit(void* memory, memory_size size);
typedef void  OSMemoryDecommit(void* memory, memory_size size);
typedef void  OSMemoryRelease(void* memory);
typedef void* OSMemoryHeapAllocate(memory_size size);
typedef void* OSMemoryHeapReallocate(void* memory, memory_size size);
typedef void  OSMemoryHeapRelease(void* memory);

typedef struct OSMemory
{
    OSMemoryReserve* reserve;
    OSMemoryCommit* commit;
    OSMemoryDecommit* decommit;
    OSMemoryRelease* release;
    OSMemoryHeapAllocate* heap_allocate;
    OSMemoryHeapReallocate* heap_reallocate;
    OSMemoryHeapRelease* heap_release;
} OSMemory;

#ifdef WIN32
#include "win32_memory.h"

static OSMemory os_memory =
{
    .reserve = &win32_memory_reserve,
    .commit = &win32_memory_commit,
    .decommit = &win32_memory_decommit,
    .release = &win32_memory_release,
    .heap_allocate = &win32_memory_heap_allocate,
    .heap_reallocate = &win32_memory_heap_reallocate,
    .heap_release = &win32_memory_heap_release,
};

#else
#error WIN32 must be defined.
#endif

void* os_memory_reserve(memory_size size)
{
    void* result = NULL;
    
    ASSERT(os_memory.reserve);
    result = os_memory.reserve(size);

    return result;
}

void os_memory_commit(void* memory, memory_size size)
{
    ASSERT(os_memory.commit);
    os_memory.commit(memory, size);
}

void os_memory_decommit(void* memory, memory_size size)
{
    ASSERT(os_memory.decommit);
    os_memory.decommit(memory, size);
}

void* os_memory_allocate(memory_size size)
{
    void* result = NULL;
    
    result = os_memory_reserve(size);
    os_memory_commit(result, size);

    return result;
}

void os_memory_release(void* memory)
{
    ASSERT(os_memory.release);
    os_memory.release(memory);
}

void* os_memory_heap_allocate(memory_size size)
{
    void* result = NULL;
    
    ASSERT(os_memory.heap_allocate);
    result = os_memory.heap_allocate(size);

    return result;
}

void* os_memory_heap_reallocate(void* memory, memory_size size)
{
    void* result = NULL;

    ASSERT(os_memory.heap_reallocate);
    result = os_memory.heap_reallocate(memory, size);

    return result;
}

void os_memory_heap_release(void* memory)
{
    ASSERT(os_memory.heap_release);
    os_memory.heap_release(memory);
}
