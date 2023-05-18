#include <Windows.h>
#include "types.h"
#include "utils.h"
#include "win32_memory.h"

#define DEFAULT_RESERVE_SIZE  MEGABYTES(1)

static SYSTEM_INFO system_info = { 0 };
static b32 system_info_read = FALSE;

static memory_size get_page_size(void)
{
    if (!system_info_read)
    {
        system_info_read = TRUE;
        GetSystemInfo(&system_info);
    }

    return system_info.dwPageSize;
}

void* win32_memory_reserve(memory_size size)
{
    void* result = NULL;
    memory_size snapped_size = 0;

    snapped_size = size;
    snapped_size += (DEFAULT_RESERVE_SIZE - 1) + snapped_size;
    snapped_size -= snapped_size % DEFAULT_RESERVE_SIZE;
    
    result = VirtualAlloc(0, snapped_size, MEM_RESERVE, PAGE_NOACCESS);
    ASSERT(result);

    return result;
}

void* win32_memory_commit(void* memory, memory_size size)
{
    void* result = NULL;
    memory_size snapped_size = 0;
    memory_size page_size = 0;

    page_size = get_page_size();
    snapped_size = size;
    snapped_size += (page_size - 1) + snapped_size;
    snapped_size -= snapped_size % page_size;

    result = VirtualAlloc(memory, snapped_size, MEM_COMMIT, PAGE_READWRITE);
    ASSERT(result);

    return result;
}

b32 win32_memory_decommit(void* memory, memory_size size)
{
    b32 result = VirtualFree(memory, size, MEM_DECOMMIT);
    ASSERT(result);

    return !!result;
}

b32 win32_memory_release(void* memory)
{
    b32 result = VirtualFree(memory, 0, MEM_RELEASE);
    ASSERT(result);

    return !!result;
}

void* win32_memory_heap_allocate(memory_size size, b32 zero_init)
{
    void* result = NULL;

    result = HeapAlloc(GetProcessHeap(), zero_init ? HEAP_ZERO_MEMORY : 0, size);
    ASSERT(result);

    return result;
}

void* win32_memory_heap_reallocate(void* memory, memory_size size, b32 zero_init)
{
    void* result = NULL;

    result = HeapReAlloc(GetProcessHeap(), zero_init ? HEAP_ZERO_MEMORY : 0, memory, size);
    ASSERT(result);

    return result;
}

b32 win32_memory_heap_release(void* memory)
{
    b32 result = FALSE;
    
    result = HeapFree(GetProcessHeap(), 0, memory);
    ASSERT(result);

    return !!result;
}
