#include <Windows.h>

#include "types.h"
#include "utils.h"
#include "memory_utils.h"
#include "os_time.h"
#include "os_io.h"
#include "win32_memory.h"
#include "win32_io.h"

typedef struct Win32IOFileFindInfo
{
    HANDLE handle;
    WIN32_FIND_DATA data;
} Win32IOFileFindInfo;

static HANDLE std_output = 0;

static DWORD map_access_mode(i32 access_mode)
{
    DWORD generic_access_mode = 0;
    
    switch (access_mode)
    {
        case OS_IO_FILE_ACCESS_MODE_READ:
        {
            generic_access_mode = GENERIC_READ;
        }
        break;
        case OS_IO_FILE_ACCESS_MODE_WRITE:
        {
            generic_access_mode = GENERIC_WRITE;
        }
        break;
        case OS_IO_FILE_ACCESS_MODE_WRITE | OS_IO_FILE_ACCESS_MODE_READ:
        {
            generic_access_mode = GENERIC_WRITE | GENERIC_READ;
        }
        break;
    }

    return generic_access_mode;
}

static uptr create_file(const char* file_name, i32 access_mode, i32 creating_mode)
{
    HANDLE file = 0;
    DWORD generic_access_mode = 0;

    generic_access_mode = map_access_mode(access_mode);

    file = CreateFile(file_name, generic_access_mode, 0, 0, creating_mode, FILE_ATTRIBUTE_NORMAL, 0);
    ASSERT(INVALID_HANDLE_VALUE != file);

    if (file == INVALID_HANDLE_VALUE)
    {
        file = 0;
    }

    return (uptr)file;
}

static memory_size write_file(uptr file, const char* buffer, memory_size size)
{
    HANDLE handle = (HANDLE)file;
    DWORD bytes_written = 0;
    memory_size offset = 0;

    while (offset < size)
    {
        DWORD write_size = ((size - offset) > U32_MAX) ? U32_MAX : (DWORD)(size - offset);

        if (!WriteFile(handle, buffer + offset, write_size, &bytes_written, 0))
        {
            break;
        }

        offset += bytes_written;
    }

    ASSERT(offset == size);

    return offset;
}

static memory_size read_file(uptr file, char* buffer, memory_size size)
{
    HANDLE handle = (HANDLE)file;
    DWORD bytes_read = 0;
    memory_size offset = 0;

    while (offset < size)
    {
        u32 read_size = ((size - offset) > U32_MAX) ? U32_MAX : (u32)(size - offset);

        if (!ReadFile(handle, buffer + offset, read_size, &bytes_read, 0))
        {
            break;
        }

        offset += bytes_read;
    }

    ASSERT(offset == size);

    return offset;
}

static u32 move_file_pointer(uptr file, i32 distance, i32 offset)
{
    u32 result = 0;
    HANDLE handle = (HANDLE)file;
    
    result = (u32)SetFilePointer(handle, distance, 0, (u32)offset);
    ASSERT(result != INVALID_SET_FILE_POINTER);

    return result;
}

// TODO: Probably we can merge win32_io_console_write() and win32_io_file_write(). At least partially?
u32 win32_io_console_write(const char* str, u32 length)
{
    DWORD number_of_chars_written = 0;

    WriteFile(std_output, str, length, &number_of_chars_written, 0);
    ASSERT(length == number_of_chars_written);

    return (u32)number_of_chars_written;
}

uptr win32_io_file_create(const char* file_name, i32 access_mode)
{
    uptr file = 0;
    
    file = create_file(file_name, access_mode, CREATE_ALWAYS);

    return file;
}

uptr win32_io_file_open(const char* file_name, i32 access_mode)
{
    uptr file = 0;
    
    file = create_file(file_name, access_mode, OPEN_ALWAYS);

    return file;
}

b32 win32_io_file_close(uptr file)
{
    BOOL result = FALSE;
    HANDLE handle = (HANDLE)file;
    
    result = CloseHandle(handle);
    ASSERT(result);

    return (b32)result;
}

b32 win32_io_file_delete(const char* file_name)
{
    BOOL result = FALSE;

    result = DeleteFile(file_name);
    ASSERT(result);

    return (b32)result;
}

memory_size win32_io_file_write(uptr file, const char* buffer, memory_size size)
{
    memory_size result = 0;

    result = write_file(file, buffer, size);

    return result;
}

memory_size win32_io_file_read_by_size(uptr file, char* buffer, memory_size size)
{
    memory_size result = 0;

    result = read_file(file, buffer, size);

    return result;
}

OSIOFileContent win32_io_file_read_by_name(MemoryArena* arena, const char* file_name)
{
    OSIOFileContent result = { 0 };
    u8* data = 0;
    memory_size size = 0;
    memory_size read_size = 0;

    uptr file = win32_io_file_open(file_name, OS_IO_FILE_ACCESS_MODE_READ);
    ASSERT(file);

    if (file)
    {
        size = win32_io_file_size(file);
        data = PUSH_SIZE(arena, size + 1);
        data[size] = 0;

        read_size = read_file(file, (char*)data, size);
        win32_io_file_close(file);
    }

    if (read_size == size)
    {
        result.data = data;
        result.size = size;
    }

    return result;
}

memory_size win32_io_file_size(uptr file)
{
    memory_size result = 0;
    u32 file_size_high = 0;
    HANDLE handle = (HANDLE)file;
    
    result = GetFileSize(handle, (DWORD*)&file_size_high);
    ASSERT(result != INVALID_FILE_SIZE);

    if (result == INVALID_FILE_SIZE)
    {
        result = 0;
        file_size_high = 0;
    }
    
    result = ((memory_size)file_size_high << 32) | result;

    return result;
}

void win32_io_file_find(OSIOFileFound* file_found, const char* file_name)
{
    WIN32_FIND_DATA data = { 0 };
    HANDLE handle = FindFirstFile(file_name, &data);

    if (handle != INVALID_HANDLE_VALUE)
    {
        do
        {
            memory_copy(file_found->file_names + file_found->file_count++, data.cFileName, string_length(data.cFileName));
        } while (FindNextFile(handle, &data) && file_found->file_count < ARRAY_COUNT(file_found->file_names));

        FindClose(handle);
    }
}

u32 win32_io_file_pointer_move(uptr file, i32 distance, i32 offset)
{
    u32 result = 0;
    
    result = move_file_pointer(file, distance, offset);

    return result;
}

u32 win32_io_file_pointer_reset(uptr file)
{
    u32 result = 0;
    
    result = move_file_pointer(file, 0, FILE_BEGIN);

    return result;
}

u32 win32_io_file_pointer_get(uptr file)
{
    u32 result = 0;
    
    result = move_file_pointer(file, 0, FILE_CURRENT);

    return result;
}

b32 win32_io_file_get_time(uptr file, OSIOFileTime* file_time)
{
    b32 result = FALSE;
    HANDLE handle = (HANDLE)file;
    FILETIME create_time = { 0 };
    FILETIME last_access_time = { 0 };
    FILETIME last_write_time = { 0 };

    result = GetFileTime(handle, &create_time, &last_access_time, &last_write_time);
    ASSERT(result);

    if (result)
    {
        file_time->create_time = ((u64)create_time.dwHighDateTime << 32) | (u64)create_time.dwLowDateTime;
        file_time->last_access_time = ((u64)last_access_time.dwHighDateTime << 32) | (u64)last_access_time.dwLowDateTime;
        file_time->last_write_time = ((u64)last_write_time.dwHighDateTime << 32) | (u64)last_write_time.dwLowDateTime;
    }

    return result;
}

void win32_io_init(void)
{
    if (!std_output)
    {
        DWORD console_mode = 0;
        HANDLE std_input = 0;

        std_output = GetStdHandle(STD_OUTPUT_HANDLE);
        
        if (!std_output)
        {
            AllocConsole();
        }

        std_output = GetStdHandle(STD_OUTPUT_HANDLE);
        std_input = GetStdHandle(STD_INPUT_HANDLE);

        ASSERT(std_output != INVALID_HANDLE_VALUE);
        ASSERT(std_input != INVALID_HANDLE_VALUE);

        GetConsoleMode(std_input, &console_mode);
        SetConsoleMode(std_input, (ENABLE_EXTENDED_FLAGS | (console_mode & (DWORD)~ENABLE_QUICK_EDIT_MODE)));
        SetConsoleMode(std_output, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}
