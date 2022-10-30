#ifndef H_OS_IO_H

#define OS_IO_MAX_OUTPUT_LENGTH 512

typedef uptr OSIOFileHandle;
typedef enum OSIOFileAccessMode
{
    OS_IO_FILE_ACCESS_MODE_NULL,
    OS_IO_FILE_ACCESS_MODE_READ,
    OS_IO_FILE_ACCESS_MODE_WRITE,
} OSIOFileAccessMode;

u32 os_io_console_write(const char* fmt, ...);
OSIOFileHandle os_io_file_create(const char* file_name, i32 access_mode);
OSIOFileHandle os_io_file_open(const char* file_name, i32 access_mode);
b32 os_io_file_delete(const char* file_name);
b32 os_io_file_close(OSIOFileHandle file_handle);

#define H_OS_IO_H
#endif
