#include "types.h"
#include "utils.h"
#include "os_io.h"

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

u32 os_io_write_console(const char* str)
{
    u32 result = 0;

    ASSERT(os_io.write_console);
    result = os_io.write_console(str, string_length(str));

    return result;
}
