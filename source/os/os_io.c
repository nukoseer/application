#include <stdio.h>
#include <stdarg.h>

#include "types.h"
#include "utils.h"
#include "os_io.h"

typedef u32  OSIOConsoleWrite(const char* str, u32 length);
typedef uptr OSIOFileCreate(const char* file_name, i32 access_mode);
typedef uptr OSIOFileOpen(const char* file_name, i32 access_mode);
typedef b32  OSIOFileClose(uptr file_handle);
typedef b32  OSIOFileDelete(const char* file_name);

typedef struct OSIO
{
    OSIOConsoleWrite* console_write;
    OSIOFileCreate* file_create;
    OSIOFileOpen* file_open;
    OSIOFileClose* file_close;
    OSIOFileDelete* file_delete;
} OSIO;

#ifdef _WIN32
#include "win32_io.h"

static OSIO os_io =
{
    .console_write = &win32_io_console_write,
    .file_create = &win32_io_file_create,
    .file_open = &win32_io_file_open,
    .file_close = &win32_io_file_close,
    .file_delete = &win32_io_file_delete,
};

#else
#error _WIN32 must be defined.
#endif

static char os_io_str[OS_IO_MAX_OUTPUT_LENGTH + 1];

static u32 write_console(const char* fmt, va_list args)
{
    u32 length = 0;
    i32 ilength = 0;

    ilength = vsnprintf(os_io_str, OS_IO_MAX_OUTPUT_LENGTH, fmt, args);
    ASSERT(ilength > 0);

    if (ilength > 0)
    {
        length = (u32)ilength;

        ASSERT(os_io.console_write);
        length = os_io.console_write(os_io_str, length);
    }

    return length;
}

u32 os_io_console_write(const char* fmt, ...)
{
    va_list args;
    u32 length = 0;
    
    va_start(args, fmt);
    length = write_console(fmt, args);
    va_end(args);
    
    return length;
}

OSIOFileHandle os_io_file_create(const char* file_name, i32 access_mode)
{
    OSIOFileHandle file_handle = 0;
    
    ASSERT(os_io.file_create);
    file_handle = (OSIOFileHandle)os_io.file_create(file_name, access_mode);

    return file_handle;
}

OSIOFileHandle os_io_file_open(const char* file_name, i32 access_mode)
{
    OSIOFileHandle file_handle = 0;
    
    ASSERT(os_io.file_open);
    file_handle = (OSIOFileHandle)os_io.file_open(file_name, access_mode);

    return file_handle;
}

b32 os_io_file_close(OSIOFileHandle file_handle)
{
    b32 result = FALSE;

    ASSERT(os_io.file_close);
    result = os_io.file_close(file_handle);

    return result;
}

b32 os_io_file_delete(const char* file_name)
{
    b32 result = FALSE;

    ASSERT(os_io.file_delete);
    result = os_io.file_delete(file_name);

    return result;
}
