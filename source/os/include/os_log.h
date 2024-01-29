#ifndef H_OS_LOG_H

typedef enum OSLogLevel
{
    OS_LOG_LEVEL_TRACE,
    OS_LOG_LEVEL_DEBUG,
    OS_LOG_LEVEL_INFO,
    OS_LOG_LEVEL_WARN,
    OS_LOG_LEVEL_ERROR,
    OS_LOG_LEVEL_FATAL,
} OSLogLevel;

// TODO: Define these macros depending on a compiler macro(debug, enable_log etc.).
#define OS_LOG_TRACE(...) os_log(OS_LOG_LEVEL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define OS_LOG_DEBUG(...) os_log(OS_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define OS_LOG_INFO(...)  os_log(OS_LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define OS_LOG_WARN(...)  os_log(OS_LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define OS_LOG_ERROR(...) os_log(OS_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define OS_LOG_FATAL(...) os_log(OS_LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)

void os_log_set_level(OSLogLevel log_level);
void os_log(OSLogLevel log_level, const char* file, i32 line, const char* fmt, ...);
void os_log_init(uptr file);

#define H_OS_LOG_H
#endif
