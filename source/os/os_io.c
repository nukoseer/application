#include <stdio.h>
#include <stdarg.h>

#include "types.h"
#include "utils.h"
#include "os_io.h"

typedef u32                OSIOConsoleWrite(const char* str, u32 length);
typedef OSIOFileHandle     OSIOFileCreate(const char* file_name, i32 access_mode);
typedef OSIOFileHandle     OSIOFileOpen(const char* file_name, i32 access_mode);
typedef b32                OSIOFileClose(OSIOFileHandle file_handle);
typedef b32                OSIOFileDelete(const char* file_name);
typedef u32                OSIOFileWrite(OSIOFileHandle file_handle, const char* buffer, u32 size);
typedef u32                OSIOFileRead(OSIOFileHandle file_handle, char* buffer, u32 size);
typedef OSIOFileFindHandle OSIOFileFindBegin(const char* file_name, u32* file_count);
typedef OSIOFileHandle     OSIOFileFindAndOpen(OSIOFileFindHandle find_handle, i32 access_mode);
typedef b32                OSIOFileFindEnd(OSIOFileFindHandle find_handle);
typedef u32                OSIOFilePointerMove(OSIOFileHandle file_handle, i32 distance, i32 offset);
typedef u32                OSIOFilePointerReset(OSIOFileHandle file_handle);
typedef u32                OSIOFilePointerGet(OSIOFileHandle file_handle);

typedef struct OSIO
{
    OSIOConsoleWrite* console_write;
    OSIOFileCreate* file_create;
    OSIOFileOpen* file_open;
    OSIOFileClose* file_close;
    OSIOFileDelete* file_delete;
    OSIOFileWrite* file_write;
    OSIOFileRead* file_read;
    OSIOFileFindBegin* file_find_begin;
    OSIOFileFindAndOpen* file_find_and_open;
    OSIOFileFindEnd* file_find_end;
    OSIOFilePointerMove* file_pointer_move;
    OSIOFilePointerReset* file_pointer_reset;
    OSIOFilePointerGet* file_pointer_get;
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
    .file_write = &win32_io_file_write,
    .file_read = &win32_io_file_read,
    .file_find_begin = &win32_io_file_find_begin,
    .file_find_and_open = &win32_io_file_find_and_open,
    .file_find_end = &win32_io_file_find_end,
    .file_pointer_move = &win32_io_file_pointer_move,
    .file_pointer_reset = &win32_io_file_pointer_reset,
    .file_pointer_get = &win32_io_file_pointer_get,
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

u32 os_io_file_write(OSIOFileHandle file_handle, const char* buffer, u32 size)
{
    u32 result = 0;

    ASSERT(os_io.file_write);
    result = os_io.file_write(file_handle, buffer, size);

    return result;
}

u32 os_io_file_read(OSIOFileHandle file_handle, char* buffer, u32 size)
{
    u32 result = 0;

    ASSERT(os_io.file_read);
    result = os_io.file_read(file_handle, buffer, size);

    return result;
}

OSIOFileFindHandle os_io_file_find_begin(const char* file_name, u32* file_count)
{
    OSIOFileFindHandle find_handle = 0;

    ASSERT(os_io.file_find_begin);
    find_handle = os_io.file_find_begin(file_name, file_count);

    return find_handle;
}

OSIOFileHandle os_io_file_find_and_open(OSIOFileFindHandle find_handle, i32 access_mode)
{
    OSIOFileHandle file_handle = 0;

    ASSERT(os_io.file_find_and_open);
    file_handle = os_io.file_find_and_open(find_handle, access_mode);

    return file_handle;
}

b32 os_io_file_find_end(OSIOFileFindHandle find_handle)
{
    b32 result = 0;

    ASSERT(os_io.file_find_end);
    result = os_io.file_find_end(find_handle);

    return result;
}

u32 os_io_file_pointer_move(OSIOFileHandle file_handle, i32 distance, OSIOFilePointerOffset offset)
{
    u32 result = 0;

    ASSERT(os_io.file_pointer_move);
    result = os_io.file_pointer_move(file_handle, distance, offset);

    return result;
}

u32 os_io_file_pointer_reset(OSIOFileHandle file_handle)
{
    u32 result = 0;

    ASSERT(os_io.file_pointer_reset);
    result = os_io.file_pointer_reset(file_handle);

    return result;
}

u32 os_io_file_pointer_get(OSIOFileHandle file_handle)
{
    u32 result = 0;

    ASSERT(os_io.file_pointer_get);
    result = os_io.file_pointer_get(file_handle);

    return result;
}
