#include "types.h"
#include "utils.h"
#include "mem.h"
#include "os_window.h"

typedef uptr OSWindowOpen(const char* title, i32 width, i32 height);
typedef void OSWindowClose(uptr window_pointer);
typedef uptr OSWindowGetHandleFrom(uptr window_pointer);
typedef uptr OSWindowGetWindowFrom(uptr handle_pointer);
typedef void OSWindowGetEventList(OSEventList* event_list, MemoryArena* event_arena);
typedef void OSWindowGetPosition(uptr window_pointer, i32* x, i32* y, i32* width, i32* height);
typedef void OsWindowSetPosition(uptr window_pointer, i32 x, i32 y, i32 width, i32 height);

typedef struct OSWindow
{
    OSWindowOpen* open;
    OSWindowClose* close;
    OSWindowGetHandleFrom* get_handle_from_window;
    OSWindowGetWindowFrom* get_window_from_handle;
    OSWindowGetEventList* get_event_list;
    OSWindowGetPosition* get_position;
    OsWindowSetPosition* set_position;
} OSWindow;

#ifdef WIN32
#include "win32_window.h"

static OSWindow os_window =
{
    .open = &win32_window_open,
    .close = &win32_window_close,
    .get_handle_from_window = &win32_window_get_handle_from,
    .get_window_from_handle = &win32_window_get_window_from,
    .get_event_list = &win32_window_get_event_list,
    .get_position = &win32_window_get_position,
    .set_position = &win32_window_set_position,
};

#else
#error WIN32 must be defined.
#endif

static MemoryArena* os_event_arena;

static OSWindowHandle get_handle_from_window(uptr window)
{
    OSWindowHandle os_window_handle = 0;
    uptr window_handle = 0;

    ASSERT(os_window.get_handle_from_window);
    window_handle = os_window.get_handle_from_window(window);

    ASSERT(window_handle);
    
    if (window_handle)
    {
        os_window_handle = (OSWindowHandle)window_handle;
    }

    return os_window_handle;
}

static uptr get_window_from_handle(uptr handle)
{
    uptr window = 0;

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

void os_window_get_position(OSWindowHandle os_window_handle, i32* x, i32* y, i32* width, i32* height)
{
    uptr window = 0;
    uptr handle = os_window_handle;

    window = get_window_from_handle(handle);

    ASSERT(os_window.get_position);
    os_window.get_position(window, x, y, width, height);
}

void os_window_set_position(OSWindowHandle os_window_handle, i32 x, i32 y, i32 width, i32 height)
{
    uptr window = 0;
    uptr handle = os_window_handle;

    window = get_window_from_handle(handle);

    ASSERT(os_window.set_position);
    os_window.set_position(window, x, y, width, height);
}

OSWindowHandle os_window_open(const char* title, i32 width, i32 height)
{
    uptr window = 0;
    OSWindowHandle os_window_handle = 0;
    
    ASSERT(os_window.open);
    window = os_window.open(title, width, height);
    os_window_handle = get_handle_from_window(window);
    ASSERT(os_window_handle);
    
    return os_window_handle;
}

void os_window_close(OSWindowHandle os_window_handle)
{
    uptr window = 0;
    uptr handle = os_window_handle;

    ASSERT(os_window.close);
    window = get_window_from_handle(handle);
    os_window.close(window);
}

void os_window_init(void)
{
    if (!os_event_arena)
    {
        os_event_arena = allocate_memory_arena(KILOBYTES(1));
    }
}
