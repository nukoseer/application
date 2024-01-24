#include "types.h"
#include "utils.h"
#include "memory_utils.h"
#include "os_window.h"

typedef uptr OSWindowOpen(const char* title, i32 x, i32 y, i32 width, i32 height, b32 borderless);
typedef b32  OSWindowClose(uptr window_pointer);
typedef uptr OSWindowGetHandleFrom(uptr window_pointer);
typedef uptr OSWindowGetWindowFrom(uptr handle_pointer);
typedef uptr OSWindowGetGraphicsHandleFrom(uptr window_pointer);
typedef OSEventList OSWindowGetEventList(MemoryArena* arena);
typedef b32  OSWindowGetPositionAndSize(uptr window_pointer, i32* x, i32* y, i32* width, i32* height);
typedef b32  OSWindowSetPositionAndSize(uptr window_pointer, i32 x, i32 y, i32 width, i32 height);
typedef b32  OSWindowSetTitle(uptr window_pointer, const char* title);
typedef f32  OSWindowGetDefaultRefreshRate(void);

typedef struct OSWindowTable
{
    OSWindowOpen* open;
    OSWindowClose* close;
    OSWindowGetHandleFrom* get_handle_from_window;
    OSWindowGetWindowFrom* get_window_from_handle;
    OSWindowGetGraphicsHandleFrom* get_graphics_handle_from_window;
    OSWindowGetEventList* get_event_list;
    OSWindowGetPositionAndSize* get_position_and_size;
    OSWindowSetPositionAndSize* set_position_and_size;
    OSWindowSetTitle* set_title;
    OSWindowGetDefaultRefreshRate* get_default_refresh_rate;
} OSWindowTable;

#ifdef _WIN32
#include "win32_window.h"

static OSWindowTable os_window_table =
{
    .open = &win32_window_open,
    .close = &win32_window_close,
    .get_handle_from_window = &win32_window_get_handle_from,
    .get_window_from_handle = &win32_window_get_window_from,
    .get_graphics_handle_from_window = &win32_window_get_graphics_handle_from,
    .get_event_list = &win32_window_get_event_list,
    .get_position_and_size = &win32_window_get_position_and_size,
    .set_position_and_size = &win32_window_set_position_and_size,
    .set_title = &win32_window_set_title,
    .get_default_refresh_rate = &win32_window_get_default_refresh_rate,
};

#else
#error _WIN32 must be defined.
#endif

static MemoryArena* os_event_arena;

static OSWindow get_handle_from_window(uptr window)
{
    OSWindow os_window = 0;

    ASSERT(window);
    
    if (window)
    {
        ASSERT(os_window_table.get_handle_from_window);
        os_window = (OSWindow)os_window_table.get_handle_from_window(window);
        ASSERT(os_window);
    }
    
    return os_window;
}

static uptr get_window_from_handle(uptr handle)
{
    uptr window = 0;

    ASSERT(handle);

    if (handle)
    {
        ASSERT(os_window_table.get_window_from_handle);
        window = os_window_table.get_window_from_handle(handle);
        ASSERT(window);
    }
    
    return window;
}

static uptr get_graphics_handle_from_window(uptr window)
{
    uptr graphics_handle = 0;

    ASSERT(os_window_table.get_graphics_handle_from_window);
    graphics_handle = os_window_table.get_graphics_handle_from_window(window);
    ASSERT(graphics_handle);

    return graphics_handle;
}

uptr os_window_get_graphics_handle(OSWindow os_window)
{
    uptr graphics_handle = 0;
    uptr window = get_window_from_handle(os_window);

    if (window)
    {
        graphics_handle = get_graphics_handle_from_window(window);
    }
    
    return graphics_handle;
}

OSEventList os_window_get_events(MemoryArena* arena)
{
    OSEventList os_event_list = { 0 };

    ASSERT(os_window_table.get_event_list);

    os_event_list = os_window_table.get_event_list(arena);

    return os_event_list;
}

b32 os_window_get_position_and_size(OSWindow os_window, i32* x, i32* y, i32* width, i32* height)
{
    b32 result = FALSE;
    uptr window = 0;
    uptr handle = os_window;

    window = get_window_from_handle(handle);

    if (window)
    {
        ASSERT(os_window_table.get_position_and_size);
        result = os_window_table.get_position_and_size(window, x, y, width, height);
    }

    return result;
}

b32 os_window_set_position_and_size(OSWindow os_window, i32 x, i32 y, i32 width, i32 height)
{
    b32 result = FALSE;
    uptr window = 0;
    uptr handle = os_window;

    window = get_window_from_handle(handle);

    if (window)
    {
        ASSERT(os_window_table.set_position_and_size);
        result = os_window_table.set_position_and_size(window, x, y, width, height);
    }

    return result;
}

b32 os_window_set_title(OSWindow os_window, const char* title)
{
    b32 result = FALSE;
    uptr window = 0;
    uptr handle = os_window;

    window = get_window_from_handle(handle);

    if (window)
    {
        ASSERT(os_window_table.set_title);
        result = os_window_table.set_title(window, title);
    }

    return result;
}

f32 os_window_get_default_refresh_rate(void)
{
    f32 result = 0;
    
    ASSERT(os_window_table.get_default_refresh_rate);
    result = os_window_table.get_default_refresh_rate();

    return result;
}

OSWindow os_window_open(const char* title, i32 x, i32 y, i32 width, i32 height, b32 borderless)
{
    uptr window = 0;
    OSWindow os_window = 0;
    
    ASSERT(os_window_table.open);
    window = os_window_table.open(title, x, y, width, height, borderless);
    ASSERT(window);
    
    os_window = get_handle_from_window(window);
    ASSERT(os_window);
    
    return os_window;
}

b32 os_window_close(OSWindow os_window)
{
    b32 result = FALSE;
    uptr window = 0;
    uptr handle = os_window;

    ASSERT(os_window_table.close);
    window = get_window_from_handle(handle);

    if (window)
    {
        result = os_window_table.close(window);
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
