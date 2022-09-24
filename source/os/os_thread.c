#include "types.h"
#include "utils.h"
#include "os_thread.h"

typedef uptr OSThreadCreate(OSThreadProcedure* thread_procedure, void* parameter);
typedef u32  OSThreadResume(uptr thread_handle);
typedef u32  OSThreadSuspend(uptr thread_handle);

typedef struct OSThread
{
    OSThreadCreate* create;
    OSThreadResume* resume;
    OSThreadSuspend* suspend;
} OSThread;

#ifdef _WIN32
#include "win32_thread.h"

static OSThread os_thread =
{
    .create = &win32_thread_create,
    .resume = &win32_thread_resume,
    .suspend = &win32_thread_suspend,
};

#else
#error _WIN32 must be defined.
#endif

OSThreadHandle os_thread_create(OSThreadProcedure* thread_procedure, void* parameter)
{
    OSThreadHandle thread_handle = 0;

    ASSERT(thread_procedure && parameter);
    ASSERT(os_thread.create);

    thread_handle = os_thread.create(thread_procedure, parameter);

    return thread_handle;
}

u32 os_thread_resume(OSThreadHandle thread_handle)
{
    u32 result = 0;

    ASSERT(os_thread.resume);
    result = os_thread.resume(thread_handle);

    return result;
}

u32 os_thread_suspend(OSThreadHandle thread_handle)
{
    u32 result = 0;

    ASSERT(os_thread.suspend);
    result = os_thread.suspend(thread_handle);

    return result;
}
