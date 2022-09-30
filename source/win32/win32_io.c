#include <Windows.h>
#include "types.h"
#include "utils.h"
#include "win32_io.h"
#include "os_io.h"

static HANDLE std_handle_output = 0;

u32 win32_io_write_console(const char* str, u32 length)
{
    DWORD number_of_chars_written = 0;

    WriteFile(std_handle_output, str, length, &number_of_chars_written, 0);
    ASSERT(length == number_of_chars_written);

    return (u32)number_of_chars_written;
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
