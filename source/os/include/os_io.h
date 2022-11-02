#ifndef H_OS_IO_H

#define OS_IO_MAX_OUTPUT_LENGTH 512

typedef uptr OSIOFileHandle;
typedef uptr OSIOFileFindHandle;

typedef enum OSIOFileAccessMode
{
    OS_IO_FILE_ACCESS_MODE_NULL,
    OS_IO_FILE_ACCESS_MODE_READ,
    OS_IO_FILE_ACCESS_MODE_WRITE,
} OSIOFileAccessMode;

typedef enum OSIOFilePointerOffset
{
    OS_IO_FILE_POINTER_OFFSET_NULL,
    OS_IO_FILE_POINTER_OFFSET_START,
    OS_IO_FILE_POINTER_OFFSET_CURRENT,
    OS_IO_FILE_POINTER_OFFSET_END,
} OSIOFilePointerOffset;

u32 os_io_console_write(const char* fmt, ...);
OSIOFileHandle os_io_file_create(const char* file_name, i32 access_mode);
OSIOFileHandle os_io_file_open(const char* file_name, i32 access_mode);
b32 os_io_file_delete(const char* file_name);
u32 os_io_file_write(OSIOFileHandle file_handle, const char* buffer, u32 size);
u32 os_io_file_read(OSIOFileHandle file_handle, char* buffer, u32 size);
b32 os_io_file_close(OSIOFileHandle file_handle);
OSIOFileFindHandle os_io_file_find_begin(const char* file_name, u32* file_count);
OSIOFileHandle os_io_file_find_and_open(OSIOFileFindHandle find_handle, i32 access_mode);
b32 os_io_file_find_end(OSIOFileFindHandle find_handle);
u32 os_io_file_pointer_move(OSIOFileHandle file_handle, i32 distance, OSIOFilePointerOffset offset);
u32 os_io_file_pointer_reset(OSIOFileHandle file_handle);
u32 os_io_file_pointer_get(OSIOFileHandle file_handle);

#define H_OS_IO_H
#endif
