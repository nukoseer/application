#include "types.h"
#include "utils.h"
#include "os_thread.h"

typedef uptr OSThreadCreate(OSThreadProcedure* thread_procedure, void* parameter);
typedef u32  OSThreadResume(uptr thread);
typedef u32  OSThreadSuspend(uptr thread);
typedef b32  OSThreadWaitOnAddress(volatile void* address, void* compare_address,
                                   memory_size address_size, u32 milliseconds);
typedef void OSThreadWakeByAddress(void* address);

typedef struct OSThreadTable
{
    OSThreadCreate* create;
    OSThreadResume* resume;
    OSThreadSuspend* suspend;
    OSThreadWaitOnAddress* wait_on_address;
    OSThreadWakeByAddress* wake_by_address;
} OSThreadTable;

#ifdef _WIN32
#include "win32_thread.h"

static OSThreadTable os_thread_table =
{
    .create = &win32_thread_create,
    .resume = &win32_thread_resume,
    .suspend = &win32_thread_suspend,
    .wait_on_address = &win32_thread_wait_on_address,
    .wake_by_address = &win32_thread_wake_by_address,
};

#else
#error _WIN32 must be defined.
#endif

OSThread os_thread_create(OSThreadProcedure* thread_procedure, void* parameter)
{
    OSThread thread = 0;

    ASSERT(thread_procedure);
    ASSERT(os_thread_table.create);

    thread = os_thread_table.create(thread_procedure, parameter);

    return thread;
}

u32 os_thread_resume(OSThread thread)
{
    u32 result = 0;

    ASSERT(os_thread_table.resume);
    result = os_thread_table.resume(thread);

    return result;
}

u32 os_thread_suspend(OSThread thread)
{
    u32 result = 0;

    ASSERT(os_thread_table.suspend);
    result = os_thread_table.suspend(thread);

    return result;
}

b32 os_thread_wait_on_address(volatile void* address, void* compare_address,
                              memory_size address_size, u32 milliseconds)
{
    b32 result = 0;

    ASSERT(os_thread_table.wait_on_address);
    result = os_thread_table.wait_on_address(address, compare_address, address_size, milliseconds);

    return result;
}

void os_thread_wake_by_address(void* address)
{
    ASSERT(os_thread_table.wake_by_address);
    os_thread_table.wake_by_address(address);
}
