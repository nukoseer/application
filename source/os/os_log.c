#include <stdio.h>
#include <stdarg.h>

#include "types.h"
#include "utils.h"
#include "os_time.h"
#include "os_io.h"
#include "os_log.h"
#include "os_atomic.h"

static volatile b32 os_log_locked = FALSE;

static const char* os_log_level_string[] = 
{
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};
static const char* os_log_level_colors[] =
{
  "\x1b[97m", "\x1b[96m", "\x1b[92m", "\x1b[93m", "\x1b[91m", "\x1b[95m"
};

static void log_lock(void)
{
    while (TRUE)
    {
        b32 locked = FALSE;

        if ((locked = os_atomic_compare_exchange_acquire(&os_log_locked, TRUE, FALSE)) == 0)
        {
            return;
        }
    }
}

static void log_unlock(void)
{
    os_atomic_compare_exchange_release(&os_log_locked, 0, os_log_locked);
}

u32 os_log(OSLogLevel log_level, const char* file, i32 line, const char* fmt, ...)
{
    u32 length = 0;
    
    log_lock();
    {
        static char buffer[OS_IO_MAX_OUTPUT_LENGTH + 1] = { 0 };
        va_list args;
        i32 ilength = 0;
        OSDateTime os_local_time = os_time_local_now();
    
        ilength = snprintf(buffer, OS_IO_MAX_OUTPUT_LENGTH,
                           "\x1b[97m%02d:%02d:%02d %s%-5s \x1b[90m%s:%d: \x1b[97m",
                           os_local_time.hour, os_local_time.minute, os_local_time.second,
                           os_log_level_colors[log_level], os_log_level_string[log_level], file, line);

        if (ilength > 0)
        {
            length += (u32)ilength;

            va_start(args, fmt);
            if ((ilength += vsnprintf(buffer + ilength, (size_t)(OS_IO_MAX_OUTPUT_LENGTH - ilength), fmt, args)) > 0)
            {
                if (length < (u32)ilength)
                {
                    length = (u32)ilength;
                }
            }
            va_end(args);

            length = os_io_write_console(buffer);
            length += os_io_write_console("\n\x1b[0m");
        }
    }
    log_unlock();

    return length;
}
