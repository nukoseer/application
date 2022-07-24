#include <Windows.h>
#include <stdio.h> // TODO: Probably we won't need this.
#include "types.h"
#include "utils.h"
#include "memory.h"
#include "os_events.h"
#include "os_window.h"
#include "os_timer.h"
#include "win32.h"

#define WINDOW_CLASS_NAME ("application_window_class")

typedef struct Win32Window
{
    HWND handle;
    HDC device_context;
    int width;
    int height;
} Win32Window;

static HINSTANCE win32_instance;
static Win32Window* win32_window;
static MemoryArena* win32_memory_arena;
static OSEventList* win32_event_list;
static MemoryArena* win32_event_arena;
    
static LRESULT CALLBACK window_proc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;
    OSEvent* event = 0;
    MemoryArena* event_arena = 0;
    OSEventList* event_list = 0;
    OSEventList proc_event_list = { 0 }; // NOTE: Windows can call window_proc whenever it needs.
    TemporaryMemory temporary_memory = { 0 };

    if (!win32_event_arena)
    {
        win32_event_list = &proc_event_list;
        win32_event_arena = win32_memory_arena;
        temporary_memory = begin_temporary_memory(win32_event_arena);
    }

    event_list = win32_event_list;
    event_arena = win32_event_arena;

    switch (message)
    {
        case WM_CLOSE:
        {
            event = push_struct(event_arena, OSEvent);
            event->type = OS_EVENT_TYPE_WINDOW_CLOSE;
            // PostQuitMessage(0);
        }
        break;

        case WM_KEYDOWN:
        {
            event = push_struct(event_arena, OSEvent);
            event->type = OS_EVENT_TYPE_PRESS;
        }
        break;

        default:
        {
            result = DefWindowProc(handle, message, wparam, lparam);
        }
    }

    if (event)
    {
        os_event_push_back(event_list, event);
    }

    if (temporary_memory.initial_size)
    {
        end_temporary_memory(temporary_memory);
    }
    
    return result;
}

static void register_window_class(void)
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

    win32_instance = instance;
}

void* win32_window_open(const char* title, int width, int height)
{
    HWND handle = 0;
    HDC device_context = 0;
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP;

    if (!win32_instance)
        register_window_class();

    if (!win32_window)
        win32_window = push_struct_zero(win32_memory_arena, Win32Window);

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
        DestroyWindow(window->handle);
    }
}

void* win32_get_handle_from_window(void* window_pointer)
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

void* win32_get_window_from_handle(void* handle_pointer)
{
    HWND handle = (HWND)handle_pointer;
    Win32Window* window = 0;
    
    ASSERT(handle);
    
    window = (Win32Window*)((LONG_PTR)GetWindowLongPtr(handle, GWLP_USERDATA));

    ASSERT(window);

    return window;
}

void win32_get_event_list(OSEventList* event_list, MemoryArena* event_arena)
{
    MSG message;
    
    win32_event_list = event_list;
    win32_event_arena = event_arena;
    
    while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }

    win32_event_list = 0;
    win32_event_arena = 0;
}

void win32_start(void)
{
    OSHandle os_handle = 0;
    b32 quit = FALSE;

    win32_memory_arena = allocate_memory_arena(1024 * 1024);

    os_init();
    os_handle = os_window_open("Application Window", 640, 480);
    
    // win32_window_open("Application Window", 640, 480);

    // OSEventList event_list = { 0 };
    // MemoryArena* event_arena = allocate_memory_arena(1024 * 2);
    while (!quit)
    {
        OSTimerHandle os_timer_handle = os_timer_begin();
        // win32_get_event_list(&event_list, event_arena);
        OSEventList event_list = os_get_events();

        Sleep(16);

        for (OSEvent* event = event_list.first; event != 0; event = event->next)
        {
            if (event->type == OS_EVENT_TYPE_WINDOW_CLOSE)
            {
                quit = TRUE;
                os_window_close(os_handle);
                break;
            }
        }

        {
            f64 milliseconds = os_timer_end(os_timer_handle);
            char timer_buffer[64] = { 0 };
        
            sprintf(timer_buffer, "%.8f ms. %d fps.\n", milliseconds, (i32)(1000.0 / milliseconds));
            OutputDebugString(timer_buffer);
        }
    }

    release_memory_arena(win32_memory_arena);
}
