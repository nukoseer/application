#ifndef H_WIN32_IO_H

u32  win32_io_console_write(const char* str, u32 length);
uptr win32_io_file_create(const char* file_name, i32 access_mode);
uptr win32_io_file_open(const char* file_name, i32 access_mode);
b32  win32_io_file_close(uptr file_handle);
b32  win32_io_file_delete(const char* file_name);
void win32_io_init(void);

#define H_WIN32_IO_H
#endif
