#include <Windows.h>

#include "types.h"
#include "utils.h"
#include "win32_io.h"
#include "os_io.h"

static HANDLE std_handle_output = 0;

static DWORD map_access_mode(i32 access_mode)
{
    DWORD generic_access_mode = 0;
    
    switch (access_mode)
    {
        case OS_IO_FILE_ACCESS_MODE_READ:
        {
            generic_access_mode = GENERIC_READ;
        }
        break;
        case OS_IO_FILE_ACCESS_MODE_WRITE:
        {
            generic_access_mode = GENERIC_WRITE;
        }
        break;
        case OS_IO_FILE_ACCESS_MODE_WRITE | OS_IO_FILE_ACCESS_MODE_READ:
        {
            generic_access_mode = GENERIC_WRITE | GENERIC_READ;
        }
        break;
    }

    return generic_access_mode;
}

static uptr create_file(const char* file_name, i32 access_mode, i32 creating_mode)
{
    HANDLE file_handle = 0;
    DWORD generic_access_mode = 0;

    generic_access_mode = map_access_mode(access_mode);

    file_handle = CreateFile(file_name, generic_access_mode, 0, 0, creating_mode, FILE_ATTRIBUTE_NORMAL, 0);
    ASSERT(INVALID_HANDLE_VALUE != file_handle);

    return (uptr)file_handle;
}

u32 win32_io_write_console(const char* str, u32 length)
{
    DWORD number_of_chars_written = 0;

    WriteFile(std_handle_output, str, length, &number_of_chars_written, 0);
    ASSERT(length == number_of_chars_written);

    return (u32)number_of_chars_written;
}

uptr win32_io_create_file(const char* file_name, i32 access_mode)
{
    uptr file_handle = 0;
    
    file_handle = create_file(file_name, access_mode, CREATE_ALWAYS);

    return file_handle;
}

uptr win32_io_open_file(const char* file_name, i32 access_mode)
{
    uptr file_handle = 0;
    
    file_handle = create_file(file_name, access_mode, OPEN_ALWAYS);

    return file_handle;
}

b32 win32_io_close_file(uptr file_handle)
{
    BOOL result = FALSE;
    HANDLE handle = (HANDLE)file_handle;
    
    result = CloseHandle(handle);
    ASSERT(result);

    return (b32)result;
}

b32 win32_io_delete_file(const char* file_name)
{
    BOOL result = FALSE;

    result = DeleteFile(file_name);
    ASSERT(result);

    return (b32)result;
}

void win32_io_init(void)
{
    if (!std_handle_output)
    {
        DWORD console_mode = 0;
        HANDLE std_handle_input = 0;

        std_handle_output = GetStdHandle(STD_OUTPUT_HANDLE);
        
        if (!std_handle_output)
        {
            AllocConsole();
        }

        std_handle_output = GetStdHandle(STD_OUTPUT_HANDLE);
        std_handle_input = GetStdHandle(STD_INPUT_HANDLE);

        ASSERT(std_handle_output != INVALID_HANDLE_VALUE);
        ASSERT(std_handle_input != INVALID_HANDLE_VALUE);

        GetConsoleMode(std_handle_input, &console_mode);
        SetConsoleMode(std_handle_input, (ENABLE_EXTENDED_FLAGS | (console_mode & (DWORD)~ENABLE_QUICK_EDIT_MODE)));
        SetConsoleMode(std_handle_output, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}
