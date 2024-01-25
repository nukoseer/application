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

typedef enum Win32IOFileTimeType
{
    WIN32_IO_FILE_TIME_TYPE_CREATION,
    WIN32_IO_FILE_TIME_TYPE_LAST_ACCESS,
    WIN32_IO_FILE_TIME_TYPE_LAST_WRITE,
} Win32IOFileTimeType;

static HANDLE std_output = 0;

static BOOL get_file_time(uptr file, Win32IOFileTimeType file_time_type, OSDateTime* os_file_time)
{
    HANDLE handle = (HANDLE)file;
    b32 result = 0;
    FILETIME file_time = { 0 };

    switch (file_time_type)
    {
        case WIN32_IO_FILE_TIME_TYPE_CREATION:
        {
            result = GetFileTime(handle, &file_time, 0, 0);
        }
        break;
        case WIN32_IO_FILE_TIME_TYPE_LAST_ACCESS:
        {
            result = GetFileTime(handle, 0, &file_time, 0);
        }
        break;
        case WIN32_IO_FILE_TIME_TYPE_LAST_WRITE:
        {
            result = GetFileTime(handle, 0, 0, &file_time);
        }
        break;
    }
    
    ASSERT(result);

    if (result)
    {
        SYSTEMTIME local_time = { 0 };
        FILETIME local_file_time = { 0 };

        result = FileTimeToLocalFileTime(&file_time, &local_file_time);
        ASSERT(result);

        if (result)
        {
            result = FileTimeToSystemTime(&local_file_time, &local_time);
        }

        ASSERT(result);

        if (result)
        {
            os_file_time->year         = local_time.wYear;
            os_file_time->month        = local_time.wMonth;
            os_file_time->day_of_week  = local_time.wDayOfWeek;
            os_file_time->day          = local_time.wDay;
            os_file_time->hour         = local_time.wHour;
            os_file_time->minute       = local_time.wMinute;
            os_file_time->second       = local_time.wSecond;
            os_file_time->milliseconds = local_time.wMilliseconds;    
        }
    }
    
    return (b32)result;
}

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

static u32 write_file(uptr file, const char* buffer, u32 size)
{
    HANDLE handle = (HANDLE)file;
    b32 result = 0;
    DWORD bytes_written = 0;

    result = WriteFile(handle, buffer, size, &bytes_written, 0);
    ASSERT(result);
    ASSERT(bytes_written == size);

    return (u32)bytes_written;
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

u32 win32_io_file_write(uptr file, const char* buffer, u32 size)
{
    u32 result = 0;

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

uptr win32_io_file_find_begin(const char* file_name, u32* file_count)
{
    Win32IOFileFindInfo* file_find_info = 0;
    u32 find_count = 0;

    file_find_info = (Win32IOFileFindInfo*)win32_memory_heap_allocate(sizeof(Win32IOFileFindInfo), TRUE);
    ASSERT(file_find_info);

    file_find_info->handle = FindFirstFile(file_name, &file_find_info->data);

    while (file_find_info->handle != INVALID_HANDLE_VALUE)
    {
        ++find_count;
        
        if (!FindNextFile(file_find_info->handle, &file_find_info->data))
        {
            break;
        }
    }

    FindClose(file_find_info->handle);
    
    file_find_info->handle = FindFirstFile(file_name, &file_find_info->data);
        
    if (file_count)
    {
        *file_count = find_count;
    }

    return (uptr)file_find_info;
}

uptr win32_io_file_find_and_open(uptr file_find, i32 access_mode)
{
    Win32IOFileFindInfo* find_info = (Win32IOFileFindInfo*)file_find;
    uptr file = 0;
    
    if (find_info->handle != INVALID_HANDLE_VALUE)
    {
        file = win32_io_file_open(find_info->data.cFileName, access_mode);

        if (!FindNextFile(find_info->handle, &find_info->data))
        {
            FindClose(find_info->handle);
            find_info->handle = INVALID_HANDLE_VALUE;
        }
    }

    return file;
}

b32 win32_io_file_find_end(uptr file_find)
{
    b32 result = 0;
    Win32IOFileFindInfo* find_info = (Win32IOFileFindInfo*)file_find;

    if (find_info)
    {
        FindClose(find_info->handle);
        result = win32_memory_heap_release(find_info);
    }
    
    return result;
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

b32 win32_io_file_get_creation_time(uptr file, OSDateTime* os_date_time)
{
    b32 result = 0;
    
    result = get_file_time(file, WIN32_IO_FILE_TIME_TYPE_CREATION, os_date_time);

    return result;
}

b32 win32_io_file_get_last_access_time(uptr file, OSDateTime* os_date_time)
{
    b32 result = 0;
    
    result = get_file_time(file, WIN32_IO_FILE_TIME_TYPE_LAST_ACCESS, os_date_time);

    return result;
}

b32 win32_io_file_get_last_write_time(uptr file, OSDateTime* os_date_time)
{
    b32 result = 0;
    
    result = get_file_time(file, WIN32_IO_FILE_TIME_TYPE_LAST_WRITE, os_date_time);

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
