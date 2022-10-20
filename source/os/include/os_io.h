#ifndef H_OS_IO_H

#define OS_IO_MAX_OUTPUT_LENGTH 512

typedef uptr OSIOFileHandle;
typedef enum OSIOFileAccessMode
{
    OS_IO_FILE_ACCESS_MODE_NULL,
    OS_IO_FILE_ACCESS_MODE_READ,
    OS_IO_FILE_ACCESS_MODE_WRITE,
} OSIOFileAccessMode;

u32 os_io_write_console(const char* fmt, ...);
OSIOFileHandle os_io_create_file(const char* file_name, i32 access_mode);
OSIOFileHandle os_io_open_file(const char* file_name, i32 access_mode);

#define H_OS_IO_H
#endif
