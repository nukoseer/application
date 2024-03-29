#include <stdio.h>
#include <stdarg.h>

#include "types.h"
#include "utils.h"
#include "memory_utils.h"
#include "os_time.h"
#include "os_io.h"

typedef OSIOFile           OSIOConsoleInit(void);
typedef OSIOFile           OSIOFileCreate(const char* file_name, i32 access_mode);
typedef OSIOFile           OSIOFileOpen(const char* file_name, i32 access_mode);
typedef b32                OSIOFileClose(OSIOFile file);
typedef b32                OSIOFileDelete(const char* file_name);
typedef memory_size        OSIOFileWrite(OSIOFile file, const char* buffer, memory_size size);
typedef memory_size        OSIOFileReadBySize(OSIOFile file, char* buffer, memory_size size);
typedef OSIOFileContent    OSIOFileReadByName(MemoryArena* arena, const char* file_name);
typedef memory_size        OSIOFileSize(OSIOFile file);
typedef void               OSIOFileFind(OSIOFileFound* file_found, const char* file_name);
typedef u32                OSIOFilePointerMove(OSIOFile file, i32 distance, i32 offset);
typedef u32                OSIOFilePointerReset(OSIOFile file);
typedef u32                OSIOFilePointerGet(OSIOFile file);
typedef b32                OSIOFileGetTime(OSIOFile file, OSIOFileTime* file_time);

typedef struct OSIOTable
{
    OSIOConsoleInit* console_init;
    OSIOFileCreate* file_create;
    OSIOFileOpen* file_open;
    OSIOFileClose* file_close;
    OSIOFileDelete* file_delete;
    OSIOFileWrite* file_write;
    OSIOFileReadBySize* file_read_by_size;
    OSIOFileReadByName* file_read_by_name;
    OSIOFileSize* file_size;
    OSIOFileFind* file_find;
    OSIOFilePointerMove* file_pointer_move;
    OSIOFilePointerReset* file_pointer_reset;
    OSIOFilePointerGet* file_pointer_get;
    OSIOFileGetTime* file_get_time;
} OSIOTable;

#ifdef _WIN32
#include "win32_io.h"

static OSIOTable os_io_table =
{
    .console_init = &win32_io_console_init,
    .file_create = &win32_io_file_create,
    .file_open = &win32_io_file_open,
    .file_close = &win32_io_file_close,
    .file_delete = &win32_io_file_delete,
    .file_write = &win32_io_file_write,
    .file_read_by_size = &win32_io_file_read_by_size,
    .file_read_by_name = &win32_io_file_read_by_name,
    .file_size = &win32_io_file_size,
    .file_find = &win32_io_file_find,
    .file_pointer_move = &win32_io_file_pointer_move,
    .file_pointer_reset = &win32_io_file_pointer_reset,
    .file_pointer_get = &win32_io_file_pointer_get,
    .file_get_time = &win32_io_file_get_time,
};

#else
#error _WIN32 must be defined.
#endif

static char os_io_str[OS_IO_MAX_OUTPUT_LENGTH + 1];

static memory_size write_console(OSIOFile file, const char* fmt, va_list args)
{
    memory_size length = 0;
    i32 ilength = 0;

    ilength = vsnprintf(os_io_str, OS_IO_MAX_OUTPUT_LENGTH, fmt, args);
    ASSERT(ilength > 0);

    if (ilength > 0)
    {
        length = (memory_size)ilength;

        ASSERT(os_io_table.file_write);
        length = os_io_table.file_write(file, os_io_str, length);
    }

    return length;
}

memory_size os_io_console_write(OSIOFile file, const char* fmt, ...)
{
    va_list args;
    memory_size length = 0;
    
    va_start(args, fmt);
    length = write_console(file, fmt, args);
    va_end(args);
    
    return length;
}

OSIOFile os_io_console_init(void)
{
    OSIOFile file = 0;

    ASSERT(os_io_table.console_init);
    file = os_io_table.console_init();

    return file;
}

OSIOFile os_io_file_create(const char* file_name, i32 access_mode)
{
    OSIOFile file = 0;
    
    ASSERT(os_io_table.file_create);
    file = (OSIOFile)os_io_table.file_create(file_name, access_mode);

    return file;
}

OSIOFile os_io_file_open(const char* file_name, i32 access_mode)
{
    OSIOFile file = 0;
    
    ASSERT(os_io_table.file_open);
    file = (OSIOFile)os_io_table.file_open(file_name, access_mode);

    return file;
}

b32 os_io_file_close(OSIOFile file)
{
    b32 result = FALSE;

    ASSERT(os_io_table.file_close);
    result = os_io_table.file_close(file);

    return result;
}

b32 os_io_file_delete(const char* file_name)
{
    b32 result = FALSE;

    ASSERT(os_io_table.file_delete);
    result = os_io_table.file_delete(file_name);

    return result;
}

memory_size os_io_file_write(OSIOFile file, const char* buffer, memory_size size)
{
    memory_size result = 0;

    ASSERT(os_io_table.file_write);
    result = os_io_table.file_write(file, buffer, size);

    return result;
}

// TODO: We need file functions directly work with file name.
memory_size os_io_file_read_by_size(OSIOFile file, char* buffer, memory_size size)
{
    memory_size result = 0;

    ASSERT(os_io_table.file_read_by_size);
    result = os_io_table.file_read_by_size(file, buffer, size);

    return result;
}

OSIOFileContent os_io_file_read_by_name(MemoryArena* arena, const char* file_name)
{
    OSIOFileContent result = { 0 };

    ASSERT(os_io_table.file_read_by_name);
    result = os_io_table.file_read_by_name(arena, file_name);

    return result;
}

memory_size os_io_file_read(OSIOFile file, char* buffer)
{
    memory_size result = 0;
    memory_size size = os_io_file_size(file);
    
    result = os_io_file_read_by_size(file, buffer, size);

    return result;
}

memory_size os_io_file_size(OSIOFile file)
{
    memory_size result = 0;

    ASSERT(os_io_table.file_size);
    result = os_io_table.file_size(file);

    return result;
}

void os_io_file_find(OSIOFileFound* file_found, const char* file_name)
{
    ASSERT(os_io_table.file_find);
    os_io_table.file_find(file_found, file_name);
}

u32 os_io_file_pointer_move(OSIOFile file, i32 distance, OSIOFilePointerOffset offset)
{
    u32 result = 0;

    ASSERT(os_io_table.file_pointer_move);
    result = os_io_table.file_pointer_move(file, distance, offset);

    return result;
}

u32 os_io_file_pointer_reset(OSIOFile file)
{
    u32 result = 0;

    ASSERT(os_io_table.file_pointer_reset);
    result = os_io_table.file_pointer_reset(file);

    return result;
}

u32 os_io_file_pointer_get(OSIOFile file)
{
    u32 result = 0;

    ASSERT(os_io_table.file_pointer_get);
    result = os_io_table.file_pointer_get(file);

    return result;
}

b32 os_io_file_get_time(OSIOFile file, OSIOFileTime* file_time)
{
    b32 result = FALSE;

    ASSERT(os_io_table.file_get_time);
    result = os_io_table.file_get_time(file, file_time);

    return result;
}
