#include <stdio.h>
#include <stdarg.h>

#include "types.h"
#include "utils.h"
#include "os_io.h"

typedef u32  OSIOWriteConsole(const char* str, u32 length);
typedef uptr OSIOCreateFile(const char* file_name, i32 access_mode);
typedef uptr OSIOOpenFile(const char* file_name, i32 access_mode);
typedef b32  OSIOCloseFile(uptr file_handle);
typedef b32  OSIODeleteFile(const char* file_name);

typedef struct OSIO
{
    OSIOWriteConsole* write_console;
    OSIOCreateFile* create_file;
    OSIOOpenFile* open_file;
    OSIOCloseFile* close_file;
    OSIODeleteFile* delete_file;
} OSIO;

#ifdef _WIN32
#include "win32_io.h"

static OSIO os_io =
{
    .write_console = &win32_io_write_console,
    .create_file = &win32_io_create_file,
    .open_file = &win32_io_open_file,
    .close_file = &win32_io_close_file,
    .delete_file = &win32_io_delete_file,
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

OSIOFileHandle os_io_create_file(const char* file_name, i32 access_mode)
{
    OSIOFileHandle file_handle = 0;
    
    ASSERT(os_io.create_file);
    file_handle = (OSIOFileHandle)os_io.create_file(file_name, access_mode);

    return file_handle;
}

OSIOFileHandle os_io_open_file(const char* file_name, i32 access_mode)
{
    OSIOFileHandle file_handle = 0;
    
    ASSERT(os_io.open_file);
    file_handle = (OSIOFileHandle)os_io.open_file(file_name, access_mode);

    return file_handle;
}

b32 os_io_close_file(OSIOFileHandle file_handle)
{
    b32 result = FALSE;

    ASSERT(os_io.close_file);
    result = os_io.close_file(file_handle);

    return result;
}

b32 os_io_delete_file(const char* file_name)
{
    b32 result = FALSE;

    ASSERT(os_io.delete_file);
    result = os_io.delete_file(file_name);

    return result;
}
