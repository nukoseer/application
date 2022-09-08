#include <stdio.h>
#include <stdarg.h>
#include "types.h"
#include "utils.h"
#include "os_io.h"

#define MAX_OUTPUT_LENGTH 512

typedef u32 OSIOWriteConsole(const char* str, u32 length);
typedef b32 OSIOSetConsoleFgColor(i32 console_fg_color);

typedef struct OSIO
{
    OSIOWriteConsole* write_console;
    OSIOSetConsoleFgColor* set_console_fg_color;
} OSIO;

#ifdef WIN32

#include "win32_io.h"

static OSIO os_io =
{
    .write_console = &win32_io_write_console,
    .set_console_fg_color = &win32_io_set_console_fg_color,
};

#else
#error WIN32 must be defined.
#endif

static char os_io_str[MAX_OUTPUT_LENGTH + 1];

static u32 write_console(const char* fmt, va_list args)
{
    u32 length = 0;
    i32 ilength = 0;

    ilength = vsnprintf(os_io_str, MAX_OUTPUT_LENGTH, fmt, args);
    ASSERT(ilength > 0);

    if (ilength > 0)
    {
        length = (u32)ilength;

        ASSERT(os_io.write_console);
        length = os_io.write_console(os_io_str, length);
    }

    return length;
}

b32 os_io_set_console_fg_color(OSIOConsoleFgColor console_fg_color)
{
    b32 result = FALSE;
    
    ASSERT(os_io.set_console_fg_color);
    result = os_io.set_console_fg_color(console_fg_color);

    return result;
}

u32 os_io_write_console(const char* fmt, ...)
{
    va_list args;
    u32 length = 0;
    
    va_start(args, fmt);
    length = write_console(fmt, args);
    va_end(args);
    
    return length;
}

