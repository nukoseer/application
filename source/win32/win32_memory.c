#include <Windows.h>
#include "types.h"
#include "utils.h"
#include "win32_memory.h"

// static SYSTEM_INFO system_info = { 0 };
// static b32 system_info_read = FALSE;

// NOTE: If we decide to do reserve and commit seperately this function may be useful?
// static u64 get_page_size(void)
// {
//     if (!system_info_read)
//     {
//         system_info_read = TRUE;
//         GetSystemInfo(&system_info);
//     }

//     return system_info.dwPageSize;
// }

void* win32_memory_allocate(u64 size)
{
    void* result = 0;

    result = VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    ASSERT(result);

    return result;
}

void win32_memory_release(void* memory)
{
    b32 result = VirtualFree(memory, 0, MEM_RELEASE);
    ASSERT(result);
}

