#include "types.h"
#include "utils.h"
#include "mem.h"
#include "os_window.h"

typedef uptr OSWindowOpen(const char* title, i32 width, i32 height);
typedef b32  OSWindowClose(uptr window_pointer);
typedef uptr OSWindowGetHandleFrom(uptr window_pointer);
typedef uptr OSWindowGetWindowFrom(uptr handle_pointer);
typedef void OSWindowGetEventList(OSEventList* event_list, MemoryArena* event_arena);
typedef b32  OSWindowGetPosition(uptr window_pointer, i32* x, i32* y, i32* width, i32* height);
typedef b32  OsWindowSetPosition(uptr window_pointer, i32 x, i32 y, i32 width, i32 height);
typedef b32  OsWindowSetPosition(uptr window_pointer, i32 x, i32 y, i32 width, i32 height);
typedef b32  OSWindowSetTitle(uptr window_pointer, const char* title);

typedef struct OSWindow
{
    OSWindowOpen* open;
    OSWindowClose* close;
    OSWindowGetHandleFrom* get_handle_from_window;
    OSWindowGetWindowFrom* get_window_from_handle;
    OSWindowGetEventList* get_event_list;
    OSWindowGetPosition* get_position;
    OsWindowSetPosition* set_position;
    OSWindowSetTitle* set_title;
} OSWindow;

#ifdef _WIN32
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
    .set_title = &win32_window_set_title,
};

#else
#error _WIN32 must be defined.
#endif

static MemoryArena* os_event_arena;

static OSWindowHandle get_handle_from_window(uptr window)
{
    OSWindowHandle os_window_handle = 0;

    ASSERT(window);
    
    if (window)
    {
        ASSERT(os_window.get_handle_from_window);
        os_window_handle = (OSWindowHandle)os_window.get_handle_from_window(window);
        ASSERT(os_window_handle);
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
        ASSERT(window);
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

b32 os_window_get_position(OSWindowHandle os_window_handle, i32* x, i32* y, i32* width, i32* height)
{
    b32 result = FALSE;
    uptr window = 0;
    uptr handle = os_window_handle;

    window = get_window_from_handle(handle);

    if (window)
    {
        ASSERT(os_window.get_position);
        result = os_window.get_position(window, x, y, width, height);
    }

    return result;
}

b32 os_window_set_position(OSWindowHandle os_window_handle, i32 x, i32 y, i32 width, i32 height)
{
    b32 result = FALSE;
    uptr window = 0;
    uptr handle = os_window_handle;

    window = get_window_from_handle(handle);

    if (window)
    {
        ASSERT(os_window.set_position);
        result = os_window.set_position(window, x, y, width, height);
    }

    return result;
}

b32 os_window_set_title(OSWindowHandle os_window_handle, const char* title)
{
    b32 result = FALSE;
    uptr window = 0;
    uptr handle = os_window_handle;

    window = get_window_from_handle(handle);

    if (window)
    {
        ASSERT(os_window.set_title);
        result = os_window.set_title(window, title);
    }

    return result;
}

OSWindowHandle os_window_open(const char* title, i32 width, i32 height)
{
    uptr window = 0;
    OSWindowHandle os_window_handle = 0;
    
    ASSERT(os_window.open);
    window = os_window.open(title, width, height);
    ASSERT(window);
    
    os_window_handle = get_handle_from_window(window);
    ASSERT(os_window_handle);
    
    return os_window_handle;
}

b32 os_window_close(OSWindowHandle os_window_handle)
{
    b32 result = FALSE;
    uptr window = 0;
    uptr handle = os_window_handle;

    ASSERT(os_window.close);
    window = get_window_from_handle(handle);

    if (window)
    {
        result = os_window.close(window);
    }

    return result;
}

void os_window_init(void)
{
    if (!os_event_arena)
    {
        os_event_arena = allocate_memory_arena(KILOBYTES(1));
    }
}
