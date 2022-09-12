#ifndef H_OS_IO_H

#define os_io_log_trace(fmt, ...) do { os_io_set_console_fg_color(CONSOLE_FG_COLOR_LWHITE); os_io_write_time(); os_io_set_console_fg_color(CONSOLE_FG_COLOR_DWHITE); os_io_write_console("%-7s", " TRACE "); os_io_set_console_fg_color(CONSOLE_FG_COLOR_GRAY); os_io_write_console("%s:%d: ", __FILE__, __LINE__); os_io_set_console_fg_color(CONSOLE_FG_COLOR_LWHITE); os_io_write_console(fmt, __VA_ARGS__); } while (0)

#define os_io_log_debug(fmt, ...) do { os_io_set_console_fg_color(CONSOLE_FG_COLOR_LWHITE); os_io_write_time(); os_io_set_console_fg_color(CONSOLE_FG_COLOR_LCYAN); os_io_write_console("%-7s", " DEBUG "); os_io_set_console_fg_color(CONSOLE_FG_COLOR_GRAY); os_io_write_console("%s:%d: ", __FILE__, __LINE__); os_io_set_console_fg_color(CONSOLE_FG_COLOR_LWHITE); os_io_write_console(fmt, __VA_ARGS__); } while (0)

#define os_io_log_info(fmt, ...) do { os_io_set_console_fg_color(CONSOLE_FG_COLOR_LWHITE); os_io_write_time(); os_io_set_console_fg_color(CONSOLE_FG_COLOR_LGREEN); os_io_write_console("%-7s", " INFO "); os_io_set_console_fg_color(CONSOLE_FG_COLOR_GRAY); os_io_write_console("%s:%d: ", __FILE__, __LINE__); os_io_set_console_fg_color(CONSOLE_FG_COLOR_LWHITE); os_io_write_console(fmt, __VA_ARGS__); } while (0)

#define os_io_log_warn(fmt, ...) do { os_io_set_console_fg_color(CONSOLE_FG_COLOR_LWHITE); os_io_write_time(); os_io_set_console_fg_color(CONSOLE_FG_COLOR_LYELLOW); os_io_write_console("%-7s", " WARN "); os_io_set_console_fg_color(CONSOLE_FG_COLOR_GRAY); os_io_write_console("%s:%d: ", __FILE__, __LINE__); os_io_set_console_fg_color(CONSOLE_FG_COLOR_LWHITE); os_io_write_console(fmt, __VA_ARGS__); } while (0)

#define os_io_log_error(fmt, ...) do { os_io_set_console_fg_color(CONSOLE_FG_COLOR_LWHITE); os_io_write_time(); os_io_set_console_fg_color(CONSOLE_FG_COLOR_LRED); os_io_write_console("%-7s", " ERROR "); os_io_set_console_fg_color(CONSOLE_FG_COLOR_GRAY); os_io_write_console("%s:%d: ", __FILE__, __LINE__); os_io_set_console_fg_color(CONSOLE_FG_COLOR_LWHITE); os_io_write_console(fmt, __VA_ARGS__); } while (0)

#define os_io_log_fatal(fmt, ...) do { os_io_set_console_fg_color(CONSOLE_FG_COLOR_LWHITE); os_io_write_time(); os_io_set_console_fg_color(CONSOLE_FG_COLOR_LPURPLE); os_io_write_console("%-7s", " FATAL "); os_io_set_console_fg_color(CONSOLE_FG_COLOR_GRAY); os_io_write_console("%s:%d: ", __FILE__, __LINE__); os_io_set_console_fg_color(CONSOLE_FG_COLOR_LWHITE); os_io_write_console(fmt, __VA_ARGS__); } while (0)

typedef enum OSIOConsoleFgColor
{
    CONSOLE_FG_COLOR_DBLUE,
    CONSOLE_FG_COLOR_DGREEN,
    CONSOLE_FG_COLOR_DCYAN,
    CONSOLE_FG_COLOR_DRED,
    CONSOLE_FG_COLOR_DPURPLE,
    CONSOLE_FG_COLOR_DYELLOW,
    CONSOLE_FG_COLOR_DWHITE,
    CONSOLE_FG_COLOR_GRAY,
    CONSOLE_FG_COLOR_LBLUE,
    CONSOLE_FG_COLOR_LGREEN,
    CONSOLE_FG_COLOR_LCYAN,
    CONSOLE_FG_COLOR_LRED,
    CONSOLE_FG_COLOR_LPURPLE,
    CONSOLE_FG_COLOR_LYELLOW,
    CONSOLE_FG_COLOR_LWHITE,
} OSIOConsoleFgColor;

b32 os_io_set_console_fg_color(OSIOConsoleFgColor console_fg_color);
u32 os_io_write_console(const char* fmt, ...);
u32 os_io_write_time(void);

#define H_OS_IO_H
#endif
