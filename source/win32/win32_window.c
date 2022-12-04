#include <Windows.h>

#pragma warning(push, 0)
#include <windowsx.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <VersionHelpers.h>
#pragma warning(pop)

#include "types.h"
#include "utils.h"
#include "mem.h"
#include "os_window.h"
#include "os_log.h"
#include "win32_window.h"

#define WINDOW_CLASS_NAME ("application_window_class")
#define MSG_WINDOW_OPEN   (WM_USER + 1)
#define MSG_WINDOW_CLOSE  (MSG_WINDOW_OPEN + 1)

#ifndef WM_NCUAHDRAWCAPTION
#define WM_NCUAHDRAWCAPTION (0x00AE)
#endif
#ifndef WM_NCUAHDRAWFRAME
#define WM_NCUAHDRAWFRAME (0x00AF)
#endif

typedef struct Win32Window Win32Window;
struct Win32Window
{
    HANDLE semaphore_handle;
    HWND handle;
    HDC device_context;
    i32 x;
    i32 y;
    i32 width;
    i32 height;
    const char* title;
    Win32Window* next;
    RECT region;
    b32 borderless;
    PADDING(4);
};

typedef struct MPARAM MPARAM;
struct MPARAM
{
    HWND handle;
    WPARAM wparam;
    MPARAM* next;
};

typedef struct MPARAMFreeList
{
    MPARAM* first;
    b32 is_busy;
    PADDING(4);
} MPARAMFreeList;

static DWORD main_thread_id;
static DWORD window_thread_id;

static MPARAMFreeList win32_window_mparam_free_list;
static Win32Window* win32_window_free_list;
static MemoryArena* win32_window_memory_arena;
static OSEventList* win32_window_event_list;
static MemoryArena* win32_window_event_arena;
static u32 win32_window_count;

static b32 win32_quit;

static void acquire_mparam_free_list(void)
{
    while (TRUE)
    {
        b32 busy = 0;

        if ((busy = InterlockedCompareExchangeAcquire((volatile long*)&win32_window_mparam_free_list.is_busy, 1, 0)) == 0)
        {
            return;
        }
        OS_LOG_WARN("acquire_mparam_free_list WaitOnAddress");
        WaitOnAddress((volatile void*)&win32_window_mparam_free_list.is_busy, &busy, sizeof(busy), INFINITE);
    }
}

static void release_mparam_free_list(void)
{
    InterlockedCompareExchangeRelease((volatile long*)&win32_window_mparam_free_list.is_busy, 0, win32_window_mparam_free_list.is_busy);
    WakeByAddressSingle(&win32_window_mparam_free_list.is_busy);
}

static void send_mparam(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
{
    acquire_mparam_free_list();
    {
        MPARAM* mparam = 0;

        if (win32_window_mparam_free_list.first)
        {
            mparam = win32_window_mparam_free_list.first;
            win32_window_mparam_free_list.first = win32_window_mparam_free_list.first->next;
        }
        else
        {
            mparam = PUSH_STRUCT(win32_window_memory_arena, MPARAM);
        }

        STRUCT_ZERO(mparam, MPARAM);
        mparam->handle = handle;
        mparam->wparam = wparam;

        PostThreadMessage(main_thread_id, message, (WPARAM)mparam, lparam);
    }
    release_mparam_free_list();
}

static void free_mparam(MPARAM* mparam)
{
    acquire_mparam_free_list();
    {
        MPARAM* temp = 0;

        temp = win32_window_mparam_free_list.first;
        win32_window_mparam_free_list.first = mparam;
        win32_window_mparam_free_list.first->next = temp;
    }
    release_mparam_free_list();
}

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
        MPARAM* mparam = (MPARAM*)message.wParam;
        WPARAM wparam = mparam->wparam;
        LPARAM lparam = message.lParam;

        switch (message.message)
        {
            case WM_CLOSE:
            {
                event = PUSH_STRUCT_ZERO(event_arena, OSEvent);
                event->type = OS_EVENT_TYPE_WINDOW_CLOSE;
            }
            break;
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                static OSKey key_map[256] = { 0 };
                b32 is_down = !(lparam & (1U << 31));
                OSEventType type = is_down ? OS_EVENT_TYPE_PRESS : OS_EVENT_TYPE_RELEASE;
                OSKey key = OS_KEY_NULL;

                if (!key_map['A'])
                {
                    u32 i = 0;
                    u32 j = 0;

                    for (i = 'A', j = OS_KEY_A; i <= 'Z'; i += 1, j += 1)
                    {
                        key_map[i] = (OSKey)j;
                    }

                    for (i = '0', j = OS_KEY_0; i <= '9'; i += 1, j += 1)
                    {
                        key_map[i] = (OSKey)j;
                    }

                    for (i = VK_F1, j = OS_KEY_F1; i <= VK_F24; i += 1, j += 1)
                    {
                        key_map[i] = (OSKey)j;
                    }

                    key_map[VK_ESCAPE]  = OS_KEY_ESC;
                    key_map[VK_TAB]     = OS_KEY_TAB;
                    key_map[VK_SPACE]   = OS_KEY_SPACE;
                    key_map[VK_RETURN]  = OS_KEY_ENTER;
                    key_map[VK_CONTROL] = OS_KEY_CTRL;
                    key_map[VK_SHIFT]   = OS_KEY_SHIFT;
                    key_map[VK_MENU]    = OS_KEY_ALT;
                    key_map[VK_UP]      = OS_KEY_UP;
                    key_map[VK_LEFT]    = OS_KEY_LEFT;
                    key_map[VK_DOWN]    = OS_KEY_DOWN;
                    key_map[VK_RIGHT]   = OS_KEY_RIGHT;

                    OS_LOG_DEBUG("Key map initialized.");
                }

                if (wparam < ARRAY_COUNT(key_map))
                {
                    key = key_map[wparam];
                }

                event = PUSH_STRUCT_ZERO(event_arena, OSEvent);
                event->key = key;
                event->type = type;
            }
            break;
        }

        if (event)
        {
            Win32Window* window = (Win32Window*)((LONG_PTR)GetWindowLongPtr(mparam->handle, GWLP_USERDATA));

            event->window_handle = (OSWindowHandle)window->handle;
            ++event_list->count;
            DLL_PUSH_BACK(event_list->first, event_list->last, event);
        }

        if (mparam)
            free_mparam(mparam);
    }

    if (temporary_memory.initial_size)
    {
        end_temporary_memory(temporary_memory);
    }
}

static void update_region(Win32Window* window)
{
	RECT old_region = window->region;

	if (IsMaximized(window->handle))
    {
		WINDOWINFO window_info = { .cbSize = sizeof(window_info) };

		GetWindowInfo(window->handle, &window_info);

        // NOTE: For maximized windows, a region is needed to cut off
        // the non-client borders that hang over the edge of the
        // screen.
		window->region = (RECT)
        {
			.left = window_info.rcClient.left - window_info.rcWindow.left,
			.top = window_info.rcClient.top - window_info.rcWindow.top,
			.right = window_info.rcClient.right - window_info.rcWindow.left,
			.bottom = window_info.rcClient.bottom - window_info.rcWindow.top,
		};
	}
    else {
		// NOTE: Don't mess with the region when window is not
        // maximized, otherwise it will lose its shadow.
		window->region = (RECT) { 0 };
	}

	// NOTE: Avoid unnecessarily updating the region to avoid unnecessary redraws.
	if (EqualRect(&window->region, &old_region))
		return;
	// NOTE: Treat empty regions as NULL regions.
	if (EqualRect(&window->region, &(RECT) { 0 }))
		SetWindowRgn(window->handle, NULL, TRUE);
	else
		SetWindowRgn(window->handle, CreateRectRgnIndirect(&window->region), TRUE);
}

static void handle_windowposchanged(Win32Window* window, const WINDOWPOS* pos)
{
	RECT client = { 0 };
    LONG old_width = 0;
	LONG old_height = 0;
    b32 client_changed = 0;

	GetClientRect(window->handle, &client);

    old_width = (LONG)window->width;
	old_height = (LONG)window->height;
	window->width = client.right;
	window->height = client.bottom;
	client_changed = window->width != old_width || window->height != old_height;

	if (client_changed || (pos->flags & SWP_FRAMECHANGED))
		update_region(window);

	if (client_changed)
    {
		// NOTE: Invalidate the changed parts of the rectangle drawn in WM_PAINT.
		if (window->width > old_width)
			InvalidateRect(window->handle, &(RECT) { old_width - 1, 0, old_width, old_height }, TRUE);
        else
			InvalidateRect(window->handle, &(RECT) { window->width - 1, 0, window->width, window->height }, TRUE);

		if (window->height > old_height)
			InvalidateRect(window->handle, &(RECT) { 0, old_height - 1, old_width, old_height }, TRUE);
        else
			InvalidateRect(window->handle, &(RECT) { 0, window->height - 1, window->width, window->height }, TRUE);
	}
}

static b32 has_autohide_appbar(UINT edge, RECT monitor)
{
    b32 result = FALSE;

	if (IsWindows8Point1OrGreater())
		result = (b32)SHAppBarMessage(ABM_GETAUTOHIDEBAREX, &(APPBARDATA) { .cbSize = sizeof(APPBARDATA), .uEdge = edge, .rc = monitor });
    else
    {
        // NOTE: Before Windows 8.1, it was not possible to specify a
        // monitor when checking for hidden appbars, so check only on
        // the primary monitor.
        if (monitor.left != 0 || monitor.top != 0)
            result = FALSE;
        else
            result = (b32)SHAppBarMessage(ABM_GETAUTOHIDEBAR, &(APPBARDATA) { .cbSize = sizeof(APPBARDATA), .uEdge = edge });
    }

    return result;
}

static void handle_nccalcsize(Win32Window* window, WPARAM wparam, LPARAM lparam)
{
    HMONITOR monitor = { 0 };
    MONITORINFO monitor_info = { 0 };
    RECT nonclient = { 0 };
    RECT client = { 0 };
	union
    {
		LPARAM lparam;
		RECT* rect;
	} params = { .lparam = lparam };

    // NOTE: DefWindowProc must be called in both the maximized and
    // non-maximized cases, otherwise tile/cascade windows won't work.
	nonclient = *params.rect;
    DefWindowProcW(window->handle, WM_NCCALCSIZE, wparam, params.lparam);
	client = *params.rect;

	if (IsMaximized(window->handle))
    {
		WINDOWINFO window_info = { .cbSize = sizeof(window_info) };

		GetWindowInfo(window->handle, &window_info);

        // NOTE: Maximized windows always have a non-client border
        // that hangs over the edge of the screen, so the size
        // proposed by WM_NCCALCSIZE is fine. Just adjust the top
        // border to remove the window title.
		*params.rect = (RECT)
        {
			.left = client.left,
			.top = ((LONG)nonclient.top + (LONG)window_info.cyWindowBorders),
			.right = client.right,
			.bottom = client.bottom,
		};

		monitor = MonitorFromWindow(window->handle, MONITOR_DEFAULTTOPRIMARY);
		monitor_info.cbSize = sizeof(monitor_info);
		GetMonitorInfoW(monitor, &monitor_info);

        // NOTE: If the client rectangle is the same as the monitor's
        // rectangle, the shell assumes that the window has gone
        // fullscreen, so it removes the topmost attribute from any
        // auto-hide appbars, making them inaccessible. To avoid this,
        // reduce the size of the client area by one pixel on a
        // certain edge. The edge is chosen based on which side of the
        // monitor is likely to contain an auto-hide appbar, so the
        // missing client area is covered by it.
		if (EqualRect(params.rect, &monitor_info.rcMonitor))
        {
			if (has_autohide_appbar(ABE_BOTTOM, monitor_info.rcMonitor))
				params.rect->bottom--;
			else if (has_autohide_appbar(ABE_LEFT, monitor_info.rcMonitor))
				params.rect->left++;
			else if (has_autohide_appbar(ABE_TOP, monitor_info.rcMonitor))
				params.rect->top++;
			else if (has_autohide_appbar(ABE_RIGHT, monitor_info.rcMonitor))
				params.rect->right--;
		}
	}
    else
    {
        // NOTE: For the non-maximized case, set the output RECT to
        // what it was before WM_NCCALCSIZE modified it. This will
        // make the client size the same as the non-client size.
		*params.rect = nonclient;
	}
}

static LRESULT handle_nchittest(Win32Window* window, int x, int y)
{
    LRESULT result = HTCLIENT;

	if (!IsMaximized(window->handle))
    {
        i32 frame_size = 0;
        i32 diagonal_width = 0;
        POINT mouse = { x, y };

        ScreenToClient(window->handle, &mouse);

        // NOTE: The horizontal frame should be the same size as the
        // vertical frame, since the NONCLIENTMETRICS structure does
        // not distinguish between them.
        frame_size = GetSystemMetrics(SM_CXFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
        // NOTE: The diagonal size handles are wider than the frame.
        diagonal_width = frame_size * 2 + GetSystemMetrics(SM_CXBORDER);

        if (mouse.y < frame_size)
        {
            if (mouse.x < diagonal_width)
                result = HTTOPLEFT;
            else if (mouse.x >= window->width - diagonal_width)
                result = HTTOPRIGHT;
            else
                result = HTTOP; // maybe HTCAPTION.
        }
        else if (mouse.y >= window->height - frame_size)
        {
            if (mouse.x < diagonal_width)
                result = HTBOTTOMLEFT;
            else if (mouse.x >= window->width - diagonal_width)
                result = HTBOTTOMRIGHT;
            else
                result = HTBOTTOM;
        }
        else
        {
            if (mouse.x < frame_size)
                result = HTLEFT;
            else if (mouse.x >= window->width - frame_size)
                result = HTRIGHT;
        }
    }

	return result;
}

static LRESULT CALLBACK window_proc(HWND handle, UINT message, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;

    switch (message)
    {
        case WM_NCACTIVATE:
        {
            // NOTE: DefWindowProc won't repaint the window border if
            // lParam (normally a HRGN) is -1. This is recommended in:
            // https://blogs.msdn.microsoft.com/wpfsdk/2008/09/08/custom-window-chrome-in-wpf/
            return DefWindowProc(handle, message, wparam, -1);
        }
        case WM_NCUAHDRAWCAPTION:
        case WM_NCUAHDRAWFRAME:
        {
            // NOTE: These undocumented messages are sent to draw themed
            // window borders. Block them to prevent drawing borders over
            // the client area.
        }
        break;
        case WM_WINDOWPOSCHANGED:
        case WM_NCCALCSIZE:
        case WM_NCHITTEST:
        {
            Win32Window* window = (Win32Window*)((LONG_PTR)GetWindowLongPtr(handle, GWLP_USERDATA));

            if (window && window->borderless)
            {
                if (message == WM_WINDOWPOSCHANGED)
                {
                    handle_windowposchanged(window, (WINDOWPOS*)lparam);
                }
                else if (message == WM_NCCALCSIZE)
                {
                    handle_nccalcsize(window, wparam, lparam);
                }
                else if (message == WM_NCHITTEST)
                {
                    result = handle_nchittest(window, GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
                }
            }
            else
            {
                result = DefWindowProc(handle, message, wparam, lparam);
            }
        }
        break;
        case WM_LBUTTONDOWN:
        {
            if (wparam & MK_SHIFT)
            {
                ReleaseCapture();
                SendMessageW(handle, WM_NCLBUTTONDOWN, HTCAPTION, 0);
            }
        }
        break;
        case WM_CLOSE:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            send_mparam(handle, message, wparam, lparam);
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

// NOTE: Resource for borderless window support: https://github.com/rossy/borderless-window
static void window_open(Win32Window* win32_window)
{
    HWND handle = 0;
    HDC device_context = 0;
    DWORD style = WS_OVERLAPPEDWINDOW;
    DWORD exstyle = WS_EX_APPWINDOW;

    if (win32_window->borderless)
        exstyle |= WS_EX_LAYERED;
    else
        exstyle |= WS_EX_NOREDIRECTIONBITMAP;

    handle = CreateWindowEx(exstyle, WINDOW_CLASS_NAME,
                            win32_window->title, style,
                            win32_window->x, win32_window->y,
                            win32_window->width, win32_window->height,
                            0, 0, 0, 0);
    ASSERT(handle);
    device_context = GetDC(handle);
    ASSERT(device_context);

    win32_window->handle = handle;
    win32_window->device_context = device_context;

    SetWindowLongPtr(handle, GWLP_USERDATA, (LONG_PTR)win32_window);

    if (win32_window->borderless)
    {
        SetLayeredWindowAttributes(handle, RGB(255, 0, 255), 0, LWA_COLORKEY);
        // NOTE: The window needs a frame to show a shadow, so give it
        // the smallest amount of frame possible
        // https://learn.microsoft.com/en-us/windows/win32/api/dwmapi/nf-dwmapi-dwmenablecomposition
        DwmExtendFrameIntoClientArea(handle, &(MARGINS) { 0, 0, 1, 0 });
        DwmSetWindowAttribute(handle, DWMWA_NCRENDERING_POLICY, &(DWORD) { DWMNCRP_ENABLED }, sizeof(DWORD));
        update_region(win32_window);
    }
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
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    atom = RegisterClassEx(&window_class);
    UNUSED_VARIABLE(atom);
    ASSERT(atom);
}

static void set_process_dpi_aware(void)
{
    b32 result = 0;

    result = SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

    if (!result)
    {
        result = !!SetProcessDPIAware();
    }

    ASSERT(result);
}

uptr win32_window_open(const char* title, i32 x, i32 y, i32 width, i32 height, b32 borderless)
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
    win32_window->x = x;
    win32_window->y = y;
    win32_window->width = width;
    win32_window->height = height;
    win32_window->borderless = borderless;

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

    if (result)
    {
        window->x = x;
        window->y = y;
        window->width = width;
        window->height = height;
    }

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
    set_process_dpi_aware();
    main_thread_id = GetCurrentThreadId();
    win32_window_memory_arena = allocate_memory_arena(KILOBYTES(1));

    ASSERT(main_thread_id && win32_window_memory_arena);
}
