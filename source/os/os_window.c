#include "types.h"
#include "utils.h"
#include "mem.h"
#include "os_window.h"

typedef void* OSWindowOpen(const char* title, i32 width, i32 height);
typedef void  OSWindowClose(void* window_pointer);
typedef void* OSWindowGetHandleFrom(void* window_pointer);
typedef void* OSWindowGetWindowFrom(void* handle_pointer);
typedef void  OSWindowGetEventList(OSEventList* event_list, MemoryArena* event_arena);

typedef struct OSWindow
{
    OSWindowOpen* open;
    OSWindowClose* close;
    OSWindowGetHandleFrom* get_handle_from_window;
    OSWindowGetWindowFrom* get_window_from_handle;
    OSWindowGetEventList* get_event_list;
} OSWindow;

#ifdef WIN32
#include "win32.h"

static OSWindow os_window =
{
    .open = &win32_window_open,
    .close = &win32_window_close,
    .get_handle_from_window = &win32_window_get_handle_from,
    .get_window_from_handle = &win32_window_get_window_from,
    .get_event_list = &win32_window_get_event_list,
};

#else
#error WIN32 must be defined.
#endif

static MemoryArena* os_event_arena;

static OSHandle get_handle_from_window(void* window)
{
    OSHandle os_handle = 0;
    void* handle = 0;

    ASSERT(os_window.get_handle_from_window);
    handle = os_window.get_handle_from_window(window);

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
        ASSERT(os_window.get_window_from_handle);
        window = os_window.get_window_from_handle(handle);
    }
    
    return window;
}

OSEventList os_window_get_events(void)
{
    static OSEventList os_event_list;
    static TemporaryMemory os_temporary_memory;

    memory_zero(&os_event_list, sizeof(os_event_list));

    if (os_temporary_memory.initial_size < os_event_arena->used_size)
    {
        end_temporary_memory(os_temporary_memory);
    }
    
    os_temporary_memory = begin_temporary_memory(os_event_arena);

    ASSERT(os_window.get_event_list);

    os_window.get_event_list(&os_event_list, os_event_arena);

    return os_event_list;
}

OSHandle os_window_open(const char* title, i32 width, i32 height)
{
    void* window = 0;
    OSHandle os_handle = 0;
    
    ASSERT(os_window.open);
    window = os_window.open(title, width, height);
    os_handle = get_handle_from_window(window);
    ASSERT(os_handle);
    
    return os_handle;
}

void os_window_close(OSHandle os_handle)
{
    void* window = 0;
    void* handle = (void*)os_handle;

    ASSERT(os_window.close);
    window = get_window_from_handle(handle);
    os_window.close(window);
}

void os_window_init(void)
{
    if (!os_event_arena)
    {
        os_event_arena = allocate_memory_arena(1024 * 1024);
    }
}
