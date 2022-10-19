#ifndef H_WIN32_IO_H

u32  win32_io_write_console(const char* str, u32 length);
uptr win32_io_create_file(const char* file_name, i32 access_mode);
void win32_io_init(void);

#define H_WIN32_IO_H
#endif
