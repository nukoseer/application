#include <Windows.h>

#include "types.h"
#include "utils.h"
#include "win32_memory.h"
#include "win32_io.h"
#include "os_time.h"
#include "os_io.h"

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

static HANDLE std_handle_output = 0;

static BOOL get_file_time(uptr file_handle, Win32IOFileTimeType file_time_type, OSDateTime* os_file_time)
{
    HANDLE handle = (HANDLE)file_handle;
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
        SYSTEMTIME system_time = { 0 };

        result = FileTimeToSystemTime(&file_time, &system_time);

        ASSERT(result);

        if (result)
        {
            os_file_time->year         = system_time.wYear;
            os_file_time->month        = system_time.wMonth;
            os_file_time->day_of_week  = system_time.wDayOfWeek;
            os_file_time->day          = system_time.wDay;
            os_file_time->hour         = system_time.wHour;
            os_file_time->minute       = system_time.wMinute;
            os_file_time->second       = system_time.wSecond;
            os_file_time->milliseconds = system_time.wMilliseconds;    
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
    HANDLE file_handle = 0;
    DWORD generic_access_mode = 0;

    generic_access_mode = map_access_mode(access_mode);

    file_handle = CreateFile(file_name, generic_access_mode, 0, 0, creating_mode, FILE_ATTRIBUTE_NORMAL, 0);
    ASSERT(INVALID_HANDLE_VALUE != file_handle);

    if (file_handle == INVALID_HANDLE_VALUE)
    {
        file_handle = 0;
    }

    return (uptr)file_handle;
}

static u32 write_file(uptr file_handle, const char* buffer, u32 size)
{
    HANDLE handle = (HANDLE)file_handle;
    b32 result = 0;
    DWORD bytes_written = 0;

    result = WriteFile(handle, buffer, size, &bytes_written, 0);
    ASSERT(result);
    ASSERT(bytes_written == size);

    return (u32)bytes_written;
}

static u32 read_file(uptr file_handle, char* buffer, u32 size)
{
    HANDLE handle = (HANDLE)file_handle;
    b32 result = 0;
    DWORD bytes_read = 0;

    result = ReadFile(handle, buffer, size, &bytes_read, 0);
    ASSERT(result);
    ASSERT(bytes_read == size);

    return (u32)bytes_read;
}

static u32 move_file_pointer(uptr file_handle, i32 distance, i32 offset)
{
    u32 result = 0;
    HANDLE handle = (HANDLE)file_handle;
    
    result = (u32)SetFilePointer(handle, distance, 0, (u32)offset);
    ASSERT(result != INVALID_SET_FILE_POINTER);

    return result;
}

// TODO: Probably we can merge win32_io_console_write() and win32_io_file_write(). At least partially?
u32 win32_io_console_write(const char* str, u32 length)
{
    DWORD number_of_chars_written = 0;

    WriteFile(std_handle_output, str, length, &number_of_chars_written, 0);
    ASSERT(length == number_of_chars_written);

    return (u32)number_of_chars_written;
}

uptr win32_io_file_create(const char* file_name, i32 access_mode)
{
    uptr file_handle = 0;
    
    file_handle = create_file(file_name, access_mode, CREATE_ALWAYS);

    return file_handle;
}

uptr win32_io_file_open(const char* file_name, i32 access_mode)
{
    uptr file_handle = 0;
    
    file_handle = create_file(file_name, access_mode, OPEN_ALWAYS);

    return file_handle;
}

b32 win32_io_file_close(uptr file_handle)
{
    BOOL result = FALSE;
    HANDLE handle = (HANDLE)file_handle;
    
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

u32 win32_io_file_write(uptr file_handle, const char* buffer, u32 size)
{
    u32 result = 0;

    result = write_file(file_handle, buffer, size);

    return result;
}

u32 win32_io_file_read(uptr file_handle, char* buffer, u32 size)
{
    u32 result = 0;

    result = read_file(file_handle, buffer, size);

    return result;
}

u32 win32_io_file_size(uptr file_handle)
{
    u32 result = 0;
    HANDLE handle = (HANDLE)file_handle;
    
    result = (u32)GetFileSize(handle ,0);
    ASSERT(result != INVALID_FILE_SIZE);

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

uptr win32_io_file_find_and_open(uptr find_handle, i32 access_mode)
{
    Win32IOFileFindInfo* find_info = (Win32IOFileFindInfo*)find_handle;
    uptr file_handle = 0;
    
    if (find_info->handle != INVALID_HANDLE_VALUE)
    {
        file_handle = win32_io_file_open(find_info->data.cFileName, access_mode);

        if (!FindNextFile(find_info->handle, &find_info->data))
        {
            FindClose(find_info->handle);
            find_info->handle = INVALID_HANDLE_VALUE;
        }
    }

    return file_handle;
}

b32 win32_io_file_find_end(uptr find_handle)
{
    b32 result = 0;
    Win32IOFileFindInfo* find_info = (Win32IOFileFindInfo*)find_handle;

    if (find_info)
    {
        FindClose(find_info->handle);
        result = win32_memory_heap_release(find_info);
    }
    
    return result;
}

u32 win32_io_file_pointer_move(uptr file_handle, i32 distance, i32 offset)
{
    u32 result = 0;
    
    result = move_file_pointer(file_handle, distance, offset);

    return result;
}

u32 win32_io_file_pointer_reset(uptr file_handle)
{
    u32 result = 0;
    
    result = move_file_pointer(file_handle, 0, FILE_BEGIN);

    return result;
}

u32 win32_io_file_pointer_get(uptr file_handle)
{
    u32 result = 0;
    
    result = move_file_pointer(file_handle, 0, FILE_CURRENT);

    return result;
}

b32 win32_io_file_get_creation_time(uptr file_handle, OSDateTime* os_date_time)
{
    b32 result = 0;
    
    result = get_file_time(file_handle, WIN32_IO_FILE_TIME_TYPE_CREATION, os_date_time);

    return result;
}

b32 win32_io_file_get_last_access_time(uptr file_handle, OSDateTime* os_date_time)
{
    b32 result = 0;
    
    result = get_file_time(file_handle, WIN32_IO_FILE_TIME_TYPE_LAST_ACCESS, os_date_time);

    return result;
}

b32 win32_io_file_get_last_write_time(uptr file_handle, OSDateTime* os_date_time)
{
    b32 result = 0;
    
    result = get_file_time(file_handle, WIN32_IO_FILE_TIME_TYPE_LAST_WRITE, os_date_time);

    return result;
}

void win32_io_init(void)
{
    if (!std_handle_output)
    {
        DWORD console_mode = 0;
        HANDLE std_handle_input = 0;

        std_handle_output = GetStdHandle(STD_OUTPUT_HANDLE);
        
        if (!std_handle_output)
        {
            AllocConsole();
        }

        std_handle_output = GetStdHandle(STD_OUTPUT_HANDLE);
        std_handle_input = GetStdHandle(STD_INPUT_HANDLE);

        ASSERT(std_handle_output != INVALID_HANDLE_VALUE);
        ASSERT(std_handle_input != INVALID_HANDLE_VALUE);

        GetConsoleMode(std_handle_input, &console_mode);
        SetConsoleMode(std_handle_input, (ENABLE_EXTENDED_FLAGS | (console_mode & (DWORD)~ENABLE_QUICK_EDIT_MODE)));
        SetConsoleMode(std_handle_output, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}
