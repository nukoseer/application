#ifndef H_OS_IO_H

typedef enum OSIOLogLevel
{
    OS_IO_LOG_LEVEL_TRACE,
    OS_IO_LOG_LEVEL_DEBUG,
    OS_IO_LOG_LEVEL_INFO,
    OS_IO_LOG_LEVEL_WARN,
    OS_IO_LOG_LEVEL_ERROR,
    OS_IO_LOG_LEVEL_FATAL,
} OSIOLogLevel;

#define OS_IO_LOG_TRACE(...) os_io_log(OS_IO_LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define OS_IO_LOG_DEBUG(...) os_io_log(OS_IO_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define OS_IO_LOG_INFO(...)  os_io_log(OS_IO_LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define OS_IO_LOG_WARN(...)  os_io_log(OS_IO_LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define OS_IO_LOG_ERROR(...) os_io_log(OS_IO_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define OS_IO_LOG_FATAL(...) os_io_log(OS_IO_LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

u32 os_io_log(OSIOLogLevel log_level, const char* file, i32 line, const char* fmt, ...);
u32 os_io_write_console(const char* fmt, ...);

#define H_OS_IO_H
#endif
