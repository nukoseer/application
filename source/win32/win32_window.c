#include <Windows.h>
#include "types.h"
#include "utils.h"
#include "mem.h"
#include "os_window.h"
#include "win32_window.h"

#define WINDOW_CLASS_NAME ("application_window_class")
#define MSG_WINDOW_OPEN   (WM_USER + 1)
#define MSG_WINDOW_CLOSE  (MSG_WINDOW_OPEN + 1)

typedef struct Win32Window Win32Window;
typedef struct Win32Window
{
    HANDLE semaphore_handle;
    HWND handle;
    HDC device_context;
    int width;
    int height;
    const char* title;
    Win32Window* next;
} Win32Window;

typedef struct ExWPARAM
{
    HWND handle;
    WPARAM wparam;
} ExWPARAM;

static Win32Window* win32_window_free_list;
static MemoryArena* win32_window_memory_arena;
static OSEventList* win32_window_event_list;
static MemoryArena* win32_window_event_arena;
static u32 win32_window_count;

static DWORD main_thread_id;
static DWORD window_thread_id;

static b32 win32_quit;

// NOTE: Main thread message loop.
static void process_message(void)
{
    OSEvent* event = 0;
    MemoryArena* event_arena = 0;
    OSEventList* event_list = 0;
    TemporaryMemory temporary_memory = { 0 };
    MSG message = { 0 };

    ASSERT(win32_window_event_arena && win32_window_event_list);

    temporary_memory = begin_temporary_memory(win32_window_event_arena);
    event_list = win32_window_event_list;
    event_arena = win32_window_event_arena;

    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        switch (message.message)
        {
            case WM_CLOSE:
            {
                event = PUSH_STRUCT_ZERO(event_arena, OSEvent);
                event->type = OS_EVENT_TYPE_WINDOW_CLOSE;
            }
            break;
            case WM_KEYDOWN:
            {
                event = PUSH_STRUCT_ZERO(event_arena, OSEvent);
                event->type = OS_EVENT_TYPE_PRESS;
            }
            break;
            case WM_KEYUP:
            {
                event = PUSH_STRUCT_ZERO(event_arena, OSEvent);
                event->type = OS_EVENT_TYPE_RELEASE;
            }
            break;
        }

        if (event)
        {
            Win32Window* window = (Win32Window*)((LONG_PTR)GetWindowLongPtr(((ExWPARAM*)&message.wParam)->handle, GWLP_USERDATA));

            event->window_handle = (OSWindowHandle)window->handle;

            DLL_PUSH_BACK(event_list->first, event_list->last, event);
            ++event_list->count;
        }
    }

    if (temporary_memory.initial_size)
    {
        end_temporary_memory(temporary_memory);
    }

}

static LRESULT CALLBACK window_proc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;

    switch (message)
    {
        case WM_CLOSE:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            ExWPARAM ex_wparam =
            {
                .handle = handle,    
                .wparam = wparam,
            };

            PostThreadMessage(main_thread_id, message, *(WPARAM*)&ex_wparam, lparam);
        }
        break;

        default:
        {
            result = DefWindowProc(handle, message, wparam, lparam);
        }
        break;
    }

    return result;
}

static void window_open(Win32Window* win32_window)
{
    HWND handle = 0;
    HDC device_context = 0;
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW | WS_EX_NOREDIRECTIONBITMAP;

    handle = CreateWindowEx(exstyle, WINDOW_CLASS_NAME, win32_window->title, style, CW_USEDEFAULT, CW_USEDEFAULT, win32_window->width, win32_window->height, 0, 0, 0, 0);
    ASSERT(handle);
    device_context = GetDC(handle);
    ASSERT(device_context);

    win32_window->handle = handle;
    win32_window->device_context = device_context;

    SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)win32_window);
    ShowWindow(handle, SW_SHOW);

    ++win32_window_count;

    ReleaseSemaphore(win32_window->semaphore_handle, 1, 0);
}

static void window_close(Win32Window* win32_window)
{
    BOOL result = DestroyWindow(win32_window->handle);

    if (result)
    {
        Win32Window* temp = win32_window_free_list;

        win32_window_free_list = win32_window;
        win32_window_free_list->next = temp;
        --win32_window_count;
    }

    ReleaseSemaphore(win32_window->semaphore_handle, 1, 0);
}

// NOTE: Actual window message loop that passes messages to main thread.
static DWORD WINAPI window_thread_proc(LPVOID param)
{
    Win32Window* win32_window = (Win32Window*)param;

    // NOTE: MSDN documentation:
    // Because the system directs messages to individual windows in an application,
    // a thread must create at least one window before starting its message loop.
    window_open(win32_window);

    {
        MSG message;

        while (GetMessage(&message, 0, 0, 0))
        {
            switch (message.message)
            {
                case MSG_WINDOW_OPEN:
                {
                    win32_window = (Win32Window*)message.wParam;
                    window_open(win32_window);
                }
                break;
                case MSG_WINDOW_CLOSE:
                {
                    win32_window = (Win32Window*)message.wParam;
                    window_close(win32_window);    
                }
                break;
                default:
                {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
                break;
            }
        }
    }

    return 0;
}

static void register_window_class(void)
{
    WNDCLASSEX window_class = { 0 };
    ATOM atom = 0;

    window_class.cbSize = sizeof(window_class);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = window_proc;
    window_class.hInstance = GetModuleHandle(0);
    window_class.lpszClassName = WINDOW_CLASS_NAME;

    atom = RegisterClassEx(&window_class);
    UNUSED_VARIABLE(atom);
    ASSERT(atom);
}

uptr win32_window_open(const char* title, int width, int height)
{
    static HANDLE semaphore_handle = 0;
    static Win32Window* win32_window = 0;

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
    ASSERT(win32_window);

    win32_window->title = title;
    win32_window->width = width;
    win32_window->height = height;

    if (!win32_window_count)
    {
        semaphore_handle = CreateSemaphoreEx(0, 0, 1, 0, 0, SEMAPHORE_ALL_ACCESS);
        win32_window->semaphore_handle = semaphore_handle;
        CloseHandle(CreateThread(0, 0, window_thread_proc, win32_window, 0, &window_thread_id));

        ASSERT(window_thread_id && semaphore_handle);
    }
    else
    {
        win32_window->semaphore_handle = semaphore_handle;
        PostThreadMessage(window_thread_id, MSG_WINDOW_OPEN, (WPARAM)win32_window, 0);
    }

    WaitForSingleObjectEx(win32_window->semaphore_handle, INFINITE, FALSE);

    return (uptr)win32_window;
}

b32 win32_window_close(uptr window_pointer)
{
    Win32Window* window = (Win32Window*)window_pointer;
    b32 result = FALSE;

    ASSERT(window && window->device_context && window->handle);

    if (window->device_context)
    {
        result = ReleaseDC(window->handle, window->device_context);
        ASSERT(result);
    }

    if (result && window->handle)
    {
        u32 temp_window_count = win32_window_count;

        PostThreadMessage(window_thread_id, MSG_WINDOW_CLOSE, (WPARAM)window, 0);
        WaitForSingleObjectEx(window->semaphore_handle, INFINITE, FALSE);

        if (temp_window_count <= win32_window_count)
        {
            result = FALSE;
            ASSERT(result);
        }

        if (!win32_window_count)
        {
            win32_quit = TRUE;
        }
    }

    return !!result;
}

uptr win32_window_get_handle_from(uptr window_pointer)
{
    HWND result = 0;
    Win32Window* window = (Win32Window*)window_pointer;

    ASSERT(window && window->handle);
    result = window->handle;

    return (uptr)result;
}

uptr win32_window_get_window_from(uptr handle_pointer)
{
    HWND handle = (HWND)handle_pointer;
    Win32Window* window = 0;

    ASSERT(handle);
    window = (Win32Window*)((LONG_PTR)GetWindowLongPtr(handle, GWLP_USERDATA));
    ASSERT(window);

    return (uptr)window;
}

void win32_window_get_event_list(OSEventList* event_list, MemoryArena* event_arena)
{
    win32_window_event_list = event_list;
    win32_window_event_arena = event_arena;

    process_message();

    win32_window_event_list = 0;
    win32_window_event_arena = 0;
}

b32 win32_window_get_position(uptr window_pointer, i32* x, i32* y, i32* width, i32* height)
{
    Win32Window* window = (Win32Window*)window_pointer;
    RECT rect = { 0 };
    BOOL result = 0;

    ASSERT(window && window->handle);
    result = GetWindowRect(window->handle, &rect);
    ASSERT(result);

    if (result)
    {
        *x = rect.left;
        *y = rect.top;
        *width = rect.right - rect.left;
        *height = rect.bottom - rect.top;
    }

    return !!result;
}

b32 win32_window_set_position(uptr window_pointer, i32 x, i32 y, i32 width, i32 height)
{
    Win32Window* window = (Win32Window*)window_pointer;
    BOOL result = 0;

    ASSERT(window && window->handle);
    result = SetWindowPos(window->handle, 0, x, y, width, height, SWP_SHOWWINDOW);
    ASSERT(result);

    return !!result;
}

b32 win32_window_set_title(uptr window_pointer, const char* title)
{
    Win32Window* window = (Win32Window*)window_pointer;
    BOOL result = 0;

    ASSERT(window && window->handle);
    result = SetWindowText(window->handle, title);
    ASSERT(result);

    return !!result;
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
    register_window_class();
    main_thread_id = GetCurrentThreadId();
    win32_window_memory_arena = allocate_memory_arena(KILOBYTES(1));

    ASSERT(main_thread_id && win32_window_memory_arena);
}
