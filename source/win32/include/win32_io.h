#ifndef H_WIN32_IO_H

b32  win32_io_set_console_fg_color(i32 console_fg_color);
u32  win32_io_write_console(const char* str, u32 length);
void win32_io_init(void);

#define H_WIN32_IO_H
#endif
