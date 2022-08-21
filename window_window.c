```C
static Win32Window* win32_window_free_list;
static u32 win32_window_count;
static MemoryArena* win32_window_memory_arena;
static OSEventList* win32_window_event_list;
static MemoryArena* win32_window_event_arena;
static b32 win32_quit;
static DWORD main_thread_id;
static DWORD thread_id;

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
}

typedef struct ExWPARAM
{
    HWND handle;
    WPARAM wparam;
} ExWPARAM;

static LRESULT CALLBACK window_proc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;
    static ExWPARAM ex_wparam = { 0 };

    ex_wparam.wparam = wparam;
    ex_wparam.handle = handle;

    switch (message)
    {
        case WM_CLOSE:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            fprintf(stdout, "message: %d, handle: %llu\n", message, (uptr)ex_wparam.handle);
            PostThreadMessage(main_thread_id, message, (WPARAM)&ex_wparam, lparam);
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

static DWORD WINAPI window_thread_proc(LPVOID param)
{
    Win32Window* win32_window = (Win32Window*)param;

    window_open(win32_window);
    ReleaseSemaphore(win32_window->semaphore_handle, 1, 0);

    for (;;)
    {
        MSG message;

        while (GetMessage(&message, 0, 0, 0))
        {
            if (message.message == 1025)
            {
                win32_window = (Win32Window*)message.wParam;
                window_open(win32_window);
                ReleaseSemaphore(win32_window->semaphore_handle, 1, 0);
                fprintf(stdout, "1025\n");
            }
            else if (message.message == 1026)
            {
                win32_window = (Win32Window*)message.wParam;
                DestroyWindow(win32_window->handle);
                ReleaseSemaphore(win32_window->semaphore_handle, 1, 0);
                fprintf(stdout, "1026\n");
            }
            else
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            }
        }
    }
}

// NOTE: Main thread calls this function to open window.
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
        HANDLE thread_handle = 0;
        
        register_window_class();
        semaphore_handle =  CreateSemaphoreEx(0, 0, 1, 0, 0, SEMAPHORE_ALL_ACCESS);
        win32_window->semaphore_handle = semaphore_handle;
        main_thread_id = GetCurrentThreadId();
        thread_handle = CreateThread(0, 0, window_thread_proc, win32_window, 0, &thread_id);
        CloseHandle(thread_handle);
    }
    else
    {
        win32_window->semaphore_handle = semaphore_handle;
        PostThreadMessage(thread_id, 1025, (WPARAM)win32_window, 0);
    }

    WaitForSingleObjectEx(win32_window->semaphore_handle, INFINITE, FALSE);
    
    fprintf(stdout, "handle: %llu\n", (uptr)win32_window->handle);
    
    return (uptr)win32_window;
}

// NOTE: Main thread calls this function to close window.
b32 win32_window_close(uptr window_pointer)
{
    Win32Window* window = (Win32Window*)window_pointer;
    b32 result = FALSE;

    ASSERT(window && window->device_context && window->handle);

    if (window->device_context)
    {
        result = ReleaseDC(window->handle, window->device_context);
    }

    if (window->handle)
    {
        Win32Window* temp = win32_window_free_list;

        win32_window_free_list = window;
        win32_window_free_list->next = temp;
        --win32_window_count;

        PostThreadMessage(thread_id, 1026, (WPARAM)window, 0);
        WaitForSingleObjectEx(window->semaphore_handle, INFINITE, FALSE);
        
        if (!win32_window_count)
        {
            win32_quit = TRUE;
        }
    }

    return !!result;
}

static void process_message(void)
{
    OSEvent* event = 0;
    MemoryArena* event_arena = 0;
    OSEventList* event_list = 0;
    OSEventList proc_event_list = { 0 }; // NOTE: Windows can call window_proc whenever it needs.
    TemporaryMemory temporary_memory = { 0 };
    MSG message = { 0 };

    if (!win32_window_event_arena)
    {
        win32_window_event_list = &proc_event_list;
        win32_window_event_arena = win32_window_memory_arena;
        temporary_memory = begin_temporary_memory(win32_window_event_arena);
    }

    event_list = win32_window_event_list;
    event_arena = win32_window_event_arena;

    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        // fprintf(stdout, "message: %d\n", message.message);

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
            HWND handle = ((ExWPARAM*)message.wParam)->handle;
            // Win32Window* window = (Win32Window*)((LONG_PTR)GetWindowLongPtr(((ExWPARAM*)message.wParam)->handle, GWLP_USERDATA));
            {
                fprintf(stderr, "window error: %lu\n", GetLastError());
            }
            event->window_handle = (OSWindowHandle)handle;

            DLL_PUSH_BACK(event_list->first, event_list->last, event);
            ++event_list->count;
        }
    }

    if (temporary_memory.initial_size)
    {
        end_temporary_memory(temporary_memory);
    }
}

// NOTE: Main thread calls this in loop.
void win32_window_get_event_list(OSEventList* event_list, MemoryArena* event_arena)
{
    win32_window_event_list = event_list;
    win32_window_event_arena = event_arena;

    process_message();

    win32_window_event_list = 0;
    win32_window_event_arena = 0;
}

```
