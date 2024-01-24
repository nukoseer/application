#include <Windows.h>
#include <process.h>

#include "types.h"
#include "utils.h"
#include "win32_thread.h"

static HANDLE thread_to_handle(uptr thread)
{
    HANDLE handle = 0;
    
    ASSERT(thread);
    handle = (HANDLE)thread;

    return handle;
}

uptr win32_thread_create(unsigned long (*thread_procedure)(void*), void* parameter)
{
    HANDLE thread = 0;

    // NOTE: In the future, we should probably think more about
    // optional parameters. All of them are 0 for now.
    thread = CreateThread(0, 0, thread_procedure, parameter, 0, 0);
    ASSERT(thread);

    return (uptr)thread;
}

u32 win32_thread_resume(uptr thread)
{
    u32 result = 0;
    HANDLE handle = thread_to_handle(thread);

    ASSERT(handle);
    if (handle)
    {
        result = (u32)ResumeThread(handle);
        ASSERT(result == (u32)-1);
    }

    return result;
}

u32 win32_thread_suspend(uptr thread)
{
    u32 result = 0;
    HANDLE handle = thread_to_handle(thread);

    ASSERT(handle);

    if (handle)
    {
        result = (u32)SuspendThread(handle);
        ASSERT(result == (u32)-1);
    }

    return result;
}

b32 win32_thread_wait_on_address(volatile void* address, void* compare_address,
                                 memory_size address_size, u32 milliseconds)
{
    b32 result = 0;

    // NOTE: I think we can use futex for Linux equilavent?
    result = WaitOnAddress(address, compare_address, address_size, milliseconds);

    return result;
}

void win32_thread_wake_by_address(void* address)
{
    WakeByAddressSingle(address);
}
