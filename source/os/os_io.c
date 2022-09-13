#include <stdio.h>
#include <stdarg.h>
#include "types.h"
#include "utils.h"
#include "os_time.h"
#include "os_io.h"

#define MAX_OUTPUT_LENGTH 512

typedef u32 OSIOWriteConsole(const char* str, u32 length);

typedef struct OSIO
{
    OSIOWriteConsole* write_console;
} OSIO;

#ifdef WIN32

#include "win32_io.h"

static OSIO os_io =
{
    .write_console = &win32_io_write_console,
};

#else
#error WIN32 must be defined.
#endif

static char os_io_str[MAX_OUTPUT_LENGTH + 1];
static const char* os_io_log_level_string[] = 
{
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};
static const char* os_io_log_level_colors[] =
{
  "\x1b[37m", "\x1b[96m", "\x1b[92m", "\x1b[93m", "\x1b[91m", "\x1b[95m"
};

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

u32 os_io_write_console(const char* fmt, ...)
{
    va_list args;
    u32 length = 0;
    
    va_start(args, fmt);
    length = write_console(fmt, args);
    va_end(args);
    
    return length;
}

u32 os_io_log(OSIOLogLevel log_level, const char* file, i32 line, const char* fmt, ...)
{
    static char buffer[MAX_OUTPUT_LENGTH] = { 0 };
    va_list args;
    u32 length = 0;
    i32 ilength = 0;
    OSDateTime os_local_time = os_time_local_now();
    
    ilength = snprintf(buffer, MAX_OUTPUT_LENGTH, "\x1b[97m%02d:%02d:%02d %s%-5s \x1b[90m%s:%d: \x1b[97m", os_local_time.hour, os_local_time.minute, os_local_time.second, os_io_log_level_colors[log_level], os_io_log_level_string[log_level], file, line);

    if (ilength > 0)
    {
        length += (u32)ilength;
        va_start(args, fmt);
        
        if ((ilength += vsnprintf(buffer + ilength, (size_t)(MAX_OUTPUT_LENGTH - ilength), fmt, args)) > 0)
        {
            if (length < (u32)ilength)
            {
                length = (u32)ilength;
            }
        }

        va_end(args);
        ASSERT(os_io.write_console);
        length = os_io.write_console(buffer, length);    
    }

    return length;
}
