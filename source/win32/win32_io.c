#include <Windows.h>
#include "types.h"
#include "utils.h"
#include "win32_io.h"
#include "os_io.h"

static HANDLE std_handle_output = 0;

b32 win32_io_set_console_fg_color(i32 console_fg_color)
{
    b32 result = FALSE;
    WORD foreground_color = 0;

    switch (console_fg_color)
    {
        case CONSOLE_FG_COLOR_DBLUE:
        {
            foreground_color = FOREGROUND_BLUE;
        }
        break;
        case CONSOLE_FG_COLOR_DGREEN:
        {
            foreground_color = FOREGROUND_GREEN;
        }
        break;
        case CONSOLE_FG_COLOR_DCYAN:
        {
            foreground_color = FOREGROUND_BLUE | FOREGROUND_GREEN;
        }
        break;
        case CONSOLE_FG_COLOR_DRED:
        {
            foreground_color = FOREGROUND_RED;
        }
        break;
        case CONSOLE_FG_COLOR_DPURPLE:
        {
            foreground_color = FOREGROUND_BLUE | FOREGROUND_RED;
        }
        break;
        case CONSOLE_FG_COLOR_DYELLOW:
        {
            foreground_color = FOREGROUND_GREEN | FOREGROUND_RED;
        }
        break;
        case CONSOLE_FG_COLOR_DWHITE:
        {
            foreground_color = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
        }
        break;
        case CONSOLE_FG_COLOR_GRAY:
        {
            foreground_color = FOREGROUND_INTENSITY;
        }
        break;
        case CONSOLE_FG_COLOR_LBLUE:
        {
            foreground_color = FOREGROUND_INTENSITY | FOREGROUND_BLUE;
        }
        break;
        case CONSOLE_FG_COLOR_LGREEN:
        {
            foreground_color = FOREGROUND_INTENSITY | FOREGROUND_GREEN;
        }
        break;
        case CONSOLE_FG_COLOR_LCYAN:
        {
            foreground_color = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;
        }
        break;
        case CONSOLE_FG_COLOR_LRED:
        {
            foreground_color = FOREGROUND_INTENSITY | FOREGROUND_RED;
        }
        break;
        case CONSOLE_FG_COLOR_LPURPLE:
        {
            foreground_color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE;
        }
        break;
        case CONSOLE_FG_COLOR_LYELLOW:
        {
            foreground_color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;
        }
        break;
        case CONSOLE_FG_COLOR_LWHITE:
        {
            foreground_color = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        }
        break;
    }
    
    result = (b32)SetConsoleTextAttribute(std_handle_output, foreground_color);
    // ASSERT(result);

    return result;
}

u32 win32_io_write_console(const char* str, u32 length)
{
    BOOL result = FALSE;
    DWORD number_of_chars_written = 0;

    result = WriteFile(std_handle_output, str, length, &number_of_chars_written, 0);
    ASSERT(result);
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
    }
}
