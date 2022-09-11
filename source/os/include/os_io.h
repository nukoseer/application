#ifndef H_OS_IO_H

#define os_io_log_trace(fmt, ...) do { os_io_set_console_fg_color(OS_IO_CONSOLE_FG_COLOR_WHITE); os_io_write_time(); os_io_set_console_fg_color(OS_IO_CONSOLE_FG_COLOR_CYAN); os_io_write_console(" TRACE "); os_io_set_console_fg_color(OS_IO_CONSOLE_FG_COLOR_GRAY); os_io_write_console("%s:%d: ", __FILE__, __LINE__); os_io_set_console_fg_color(OS_IO_CONSOLE_FG_COLOR_WHITE); os_io_write_console(fmt, __VA_ARGS__); } while (0)

typedef enum OSIOConsoleFgColor
{
    OS_IO_CONSOLE_FG_COLOR_WHITE,
    OS_IO_CONSOLE_FG_COLOR_GRAY,
    OS_IO_CONSOLE_FG_COLOR_RED,
    OS_IO_CONSOLE_FG_COLOR_GREEN,
    OS_IO_CONSOLE_FG_COLOR_BLUE,
    OS_IO_CONSOLE_FG_COLOR_CYAN,
} OSIOConsoleFgColor;

b32 os_io_set_console_fg_color(OSIOConsoleFgColor console_fg_color);
u32 os_io_write_console(const char* fmt, ...);
u32 os_io_write_time(void);

#define H_OS_IO_H
#endif
