#include <Windows.h>
#include "types.h"
#include "utils.h"
#include "mem.h"
#include "os_window.h"
#include "win32_window.h"

#define WINDOW_CLASS_NAME ("application_window_class")

typedef struct Win32Window Win32Window;
typedef struct Win32Window
{
    HWND handle;
    HDC device_context;
    int width;
    int height;
    Win32Window* next;
} Win32Window;

static Win32Window* win32_window_free_list;
static u32 win32_window_count;
static MemoryArena* win32_window_memory_arena;
static OSEventList* win32_window_event_list;
static MemoryArena* win32_window_event_arena;
static b32 win32_quit;

static LRESULT CALLBACK window_proc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;
    OSEvent* event = 0;
    MemoryArena* event_arena = 0;
    OSEventList* event_list = 0;
    OSEventList proc_event_list = { 0 }; // NOTE: Windows can call window_proc whenever it needs.
    TemporaryMemory temporary_memory = { 0 };

    if (!win32_window_event_arena)
    {
        win32_window_event_list = &proc_event_list;
        win32_window_event_arena = win32_window_memory_arena;
        temporary_memory = begin_temporary_memory(win32_window_event_arena);
    }

    event_list = win32_window_event_list;
    event_arena = win32_window_event_arena;

    switch (message)
    {
        case WM_CLOSE:
        {
            event = PUSH_STRUCT(event_arena, OSEvent);
            event->type = OS_EVENT_TYPE_WINDOW_CLOSE;
        }
        break;

        case WM_KEYDOWN:
        {
            event = PUSH_STRUCT(event_arena, OSEvent);
            event->type = OS_EVENT_TYPE_PRESS;
        }
        break;

        case WM_KEYUP:
        {
            event = PUSH_STRUCT(event_arena, OSEvent);
            event->type = OS_EVENT_TYPE_RELEASE;
        }
        break;

        // case WM_QUIT:
        // {
        //     win32_should_quit = TRUE;
        //     result = DefWindowProc(handle, message, wparam, lparam);
        // }
        // break;

        default:
        {
            result = DefWindowProc(handle, message, wparam, lparam);
        }
        break;
    }

    if (event)
    {
        Win32Window* window = (Win32Window*)((LONG_PTR)GetWindowLongPtr(handle, GWLP_USERDATA));
        event->window_handle = (OSWindowHandle)window->handle;

        DLL_PUSH_BACK(event_list->first, event_list->last, event);
        ++event_list->count;
    }

    if (temporary_memory.initial_size)
    {
        end_temporary_memory(temporary_memory);
    }

    return result;
}

static HINSTANCE register_window_class(void)
{
    HINSTANCE instance = 0;
    WNDCLASSEX window_class = { 0 };
    ATOM atom = 0;

    instance = GetModuleHandle(0);

    window_class.cbSize = sizeof(window_class);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = instance;
    window_class.lpszClassName = WINDOW_CLASS_NAME;

    atom = RegisterClassEx(&window_class);
    ASSERT(atom);

    return instance;
}

void* win32_window_open(const char* title, int width, int height)
{
    static HINSTANCE win32_instance = 0;
    Win32Window* win32_window = NULL;
    HWND handle = 0;
    HDC device_context = 0;
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP;

    if (!win32_instance)
        win32_instance = register_window_class();

    if (win32_window_free_list)
    {
        win32_window = win32_window_free_list;
        win32_window_free_list = win32_window_free_list->next;
    }
    else
    {
        win32_window = PUSH_STRUCT(win32_window_memory_arena, Win32Window);
    }

    STRUCT_ZERO(win32_window, Win32Window);

    ASSERT(win32_instance && win32_window);

    handle = CreateWindowEx(exstyle, WINDOW_CLASS_NAME, title, style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, win32_instance, 0);
    ASSERT(handle);
    device_context = GetDC(handle);
    ASSERT(device_context);

    win32_window->handle = handle;
    win32_window->device_context = device_context;
    win32_window->width = width;
    win32_window->height = height;

    SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)win32_window);
    ShowWindow(handle, SW_SHOW);

    ++win32_window_count;

    return win32_window;
}

void win32_window_close(void* window_pointer)
{
    Win32Window* window = (Win32Window*)window_pointer;

    ASSERT(window);

    if (window->device_context)
    {
        ReleaseDC(window->handle, window->device_context);
    }

    if (window->handle)
    {
        Win32Window* temp = win32_window_free_list;

        win32_window_free_list = window;
        win32_window_free_list->next = temp;
        --win32_window_count;

        DestroyWindow(window->handle);

        if (!win32_window_count)
        {
            win32_quit = TRUE;
        }
    }
}

void* win32_window_get_handle_from(void* window_pointer)
{
    HWND result = 0;
    Win32Window* window = (Win32Window*)window_pointer;

    ASSERT(window);

    if (window)
    {
        result = window->handle;
    }

    return result;
}

void* win32_window_get_window_from(void* handle_pointer)
{
    HWND handle = (HWND)handle_pointer;
    Win32Window* window = 0;

    ASSERT(handle);

    window = (Win32Window*)((LONG_PTR)GetWindowLongPtr(handle, GWLP_USERDATA));

    ASSERT(window);

    return window;
}

void win32_window_get_event_list(OSEventList* event_list, MemoryArena* event_arena)
{
    MSG message;

    win32_window_event_list = event_list;
    win32_window_event_arena = event_arena;

    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    win32_window_event_list = 0;
    win32_window_event_arena = 0;
}

u32 win32_window_get_window_count(void)
{
    return win32_window_count;
}

void win32_window_destroy(void)
{
    release_memory_arena(win32_window_memory_arena);
}

void win32_window_init(void)
{
    win32_window_memory_arena = allocate_memory_arena(KILOBYTES(1));
}
