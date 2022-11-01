#ifndef H_WIN32_IO_H

u32  win32_io_console_write(const char* str, u32 length);
uptr win32_io_file_create(const char* file_name, i32 access_mode);
uptr win32_io_file_open(const char* file_name, i32 access_mode);
b32  win32_io_file_close(uptr file_handle);
b32  win32_io_file_delete(const char* file_name);
u32  win32_io_file_write(uptr file_handle, const char* buffer, u32 size);
uptr win32_io_file_find_begin(const char* file_name, u32* file_count);
uptr win32_io_file_find_and_open(uptr find_handle, i32 access_mode);
b32  win32_io_file_find_end(uptr find_handle);
void win32_io_init(void);

#define H_WIN32_IO_H
#endif
