#ifndef H_WIN32_IO_H

u32  win32_io_console_write(const char* str, u32 length);
uptr win32_io_file_create(const char* file_name, i32 access_mode);
uptr win32_io_file_open(const char* file_name, i32 access_mode);
b32  win32_io_file_close(uptr file_handle);
b32  win32_io_file_delete(const char* file_name);
u32  win32_io_file_write(uptr file_handle, const char* buffer, u32 size);
u32  win32_io_file_read(uptr file_handle, char* buffer, u32 size);
uptr win32_io_file_find_begin(const char* file_name, u32* file_count);
uptr win32_io_file_find_and_open(uptr find_handle, i32 access_mode);
b32  win32_io_file_find_end(uptr find_handle);
u32  win32_io_file_pointer_move(uptr file_handle, i32 distance, i32 offset);
u32  win32_io_file_pointer_reset(uptr file_handle);
u32  win32_io_file_pointer_get(uptr file_handle);
void win32_io_init(void);

#define H_WIN32_IO_H
#endif
