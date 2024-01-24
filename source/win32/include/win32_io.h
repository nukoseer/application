#ifndef H_WIN32_IO_H

struct OSDateTime;

u32  win32_io_console_write(const char* str, u32 length);
uptr win32_io_file_create(const char* file_name, i32 access_mode);
uptr win32_io_file_open(const char* file_name, i32 access_mode);
b32  win32_io_file_close(uptr file);
b32  win32_io_file_delete(const char* file_name);
u32  win32_io_file_write(uptr file, const char* buffer, u32 size);
u32  win32_io_file_read(uptr file, char* buffer, u32 size);
u32  win32_io_file_size(uptr file);
uptr win32_io_file_find_begin(const char* file_name, u32* file_count);
uptr win32_io_file_find_and_open(uptr file_find, i32 access_mode);
b32  win32_io_file_find_end(uptr file_find);
u32  win32_io_file_pointer_move(uptr file, i32 distance, i32 offset);
u32  win32_io_file_pointer_reset(uptr file);
u32  win32_io_file_pointer_get(uptr file);
b32 win32_io_file_get_creation_time(uptr file, struct OSDateTime* os_date_time);
b32 win32_io_file_get_last_access_time(uptr file, struct OSDateTime* os_date_time);
b32 win32_io_file_get_last_write_time(uptr file, struct OSDateTime* os_date_time);

void win32_io_init(void);

#define H_WIN32_IO_H
#endif
