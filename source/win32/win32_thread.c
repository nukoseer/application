#include <Windows.h>
#include <process.h>

#include "types.h"
#include "utils.h"
#include "win32_thread.h"

uptr win32_thread_create(u32 (*thread_procedure)(void*), void* parameter)
{
    uptr thread_handle = 0;

    // NOTE: In the future, we should probably think more about
    // optional parameters. All of them are 0 for now.
    thread_handle = _beginthreadex(0, 0, thread_procedure, parameter, 0, 0);
    ASSERT(thread_handle);

    return thread_handle;
}

void win32_thread_exit(u32 exit_code)
{
    _endthreadex(exit_code);
}

u32 win32_thread_resume(uptr thread_handle)
{
    u32 result = 0;

    ASSERT(thread_handle);
    if (thread_handle)
    {
        HANDLE handle = (HANDLE)thread_handle;
        
        result = (u32)ResumeThread(handle);
        ASSERT(result == (u32)-1);
    }

    return result;
}

u32 win32_thread_suspend(uptr thread_handle)
{
    u32 result = 0;

    ASSERT(thread_handle);
    if (thread_handle)
    {
        HANDLE handle = (HANDLE)thread_handle;

        result = (u32)SuspendThread(handle);
        ASSERT(result == (u32)-1);
    }

    return result;
}

