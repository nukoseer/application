#include "types.h"
#include "utils.h"
#include "memory.h"
#include "os_events.h"
#include "os.h"

typedef void* (*OSOpenWindow)(const char* title, i32 width, i32 height);
typedef void (*OSCloseWindow)(void* window_pointer);
typedef void* (*OSGetHandleFromWindow)(const void* window_pointer);
typedef void* (*OSGetWindowFromHandle)(const void* handle_pointer);
typedef void (*OSGetEventList)(OSEventList* event_list, MemoryArena* event_arena);

typedef struct OS
{
    OSOpenWindow open_window;
    OSCloseWindow close_window;
    OSGetHandleFromWindow get_handle_from_window;
    OSGetWindowFromHandle get_window_from_handle;
    OSGetEventList get_event_list;
} OS;

#ifdef WIN32
#include "win32.h"

static OS os =
{
    .open_window = win32_open_window,
    .close_window = win32_close_window,
    .get_handle_from_window = win32_get_handle_from_window,
    .get_window_from_handle = win32_get_window_from_handle,
    .get_event_list = win32_get_event_list,
};

#else
#error WIN32 must be defined.
#endif

static OSHandle get_handle_from_window(void* window)
{
    OSHandle os_handle = 0;
    void* handle = os.get_handle_from_window(window);

    ASSERT(handle);
    
    if (handle)
    {
        os_handle = (OSHandle)handle;
    }

    return os_handle;
}

static void* get_window_from_handle(void* handle)
{
    void* window = 0;

    ASSERT(handle);

    if (handle)
    {
        window = os.get_window_from_handle(handle);
    }
    
    return window;
}

OSEventList os_get_event_list(void)
{
    OSEventList os_event_list = { 0 };
    static MemoryArena* os_event_arena = 0;

    if (!os_event_arena)
    {
        os_event_arena = allocate_memory_arena(1024 * 1024 * 4);
    }
    
    TemporaryMemory os_temporary_memory = begin_temporary_memory(os_event_arena);

    os.get_event_list(&os_event_list, os_event_arena);

    end_temporary_memory(os_temporary_memory);

    return os_event_list;
}

OSHandle os_open_window(const char* title, i32 width, i32 height)
{
    void* window = os.open_window(title, width, height);
    OSHandle os_handle = get_handle_from_window(window);

    ASSERT(os_handle);
    
    return os_handle;
}

void os_close_window(OSHandle os_handle)
{
    void* window = 0;
    void* handle = (void*)os_handle;

    window = get_window_from_handle(handle);
    os.close_window(window);
}
