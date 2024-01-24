#include <Windows.h>

#include "types.h"
#include "win32.h"
#include "win32_window.h"
#include "win32_io.h"

// NOTE: Needed win32 libs.
#pragma comment(lib, "User32")
#pragma comment(lib, "Winmm")
#pragma comment(lib, "Synchronization")
#pragma comment(lib, "Shell32")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "Gdi32")
#pragma comment(lib, "D3D11")
#pragma comment(lib, "DXGI")
#pragma comment(lib, "DXGUID")

uptr win32_get_procedure_address(uptr module_handle, const char* procedure_name)
{
    HMODULE module = (HMODULE)module_handle;
    FARPROC result = 0;

    result = GetProcAddress(module, procedure_name);

    return (uptr)result;
}

uptr win32_load_library(const char* module_name)
{
    HMODULE result = 0;

    result = LoadLibraryA(module_name);

    return (uptr)result;
}

b32 win32_should_quit(void)
{
    return !win32_window_get_window_count();
}

void win32_destroy(void)
{
    win32_window_destroy();
}

void win32_init(void)
{
    win32_window_init();
    win32_io_init();
}
