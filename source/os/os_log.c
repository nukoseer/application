#include <stdio.h>
#include <stdarg.h>

#include "types.h"
#include "utils.h"
#include "atomic.h"
#include "os_time.h"
#include "os_io.h"
#include "os_log.h"
#include "os_thread.h"

#define OS_LOG_RING_SIZE 32
#define OS_LOG_RING_MASK OS_LOG_RING_SIZE - 1

static const char* os_log_level_string[] = 
{
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};

static const char* os_log_level_colors[] =
{
  "\x1b[97m", "\x1b[96m", "\x1b[92m", "\x1b[93m", "\x1b[91m", "\x1b[95m"
};

typedef struct OSLogRingElement
{
    char fmt[OS_IO_MAX_OUTPUT_LENGTH + 1];
    PADDING(7);
    const char* file;
    i32 line;
    OSLogLevel log_level;
} OSLogRingElement;

// TODO: How to get rid of count?
typedef struct OSLogRing
{
    OSLogRingElement elements[OS_LOG_RING_SIZE];
    volatile i32 head;
    volatile i32 tail;
    volatile i32 count;
    PADDING(4);
} OSLogRing;

static i32 log_ring_buffer_push(void);
static i32 log_ring_buffer_pop(void);
static inline void log_ring_buffer_push_commit(void);
static inline void log_ring_buffer_pop_commit(void);

static OSIOFile default_output_file;
static OSLogRing os_log_ring;
static OSLogLevel os_log_level;

static os_thread_callback_return os_log_thread_procedure(os_thread_callback_param parameter)
{
    while (TRUE)
    {
        i32 tail = 0;
        i32 count = 0;
        
        while ((count = atomic_compare_exchange(&os_log_ring.count, os_log_ring.count, os_log_ring.count)) > 0)
        {
            tail = log_ring_buffer_pop();
            {
                static char buffer[OS_IO_MAX_OUTPUT_LENGTH + 1];
                OSLogRingElement* element = os_log_ring.elements + tail;
                OSDateTime os_local_time = os_time_local_now();

                // TODO: Formatting with colors should be optional, it does not work with files.
                snprintf(buffer, OS_IO_MAX_OUTPUT_LENGTH,
                         "\x1b[97m%02d:%02d:%02d %s%-5s \x1b[90m%s:%d: \x1b[97m %s\n\x1b[0m",
                         os_local_time.hour, os_local_time.minute, os_local_time.second,
                         os_log_level_colors[element->log_level], os_log_level_string[element->log_level],
                         element->file, element->line, element->fmt);
                
                os_io_console_write(default_output_file, buffer);
            }
            log_ring_buffer_pop_commit();
        }
        os_thread_wait_on_address(&os_log_ring.count, &count, sizeof(count), 0xFFFFFFFF);
    }

    UNUSED_VARIABLE(parameter);
}

static i32 log_ring_buffer_push(void)
{
    i32 head = 0;
    i32 masked_head = 0;

    head = atomic_increment(&os_log_ring.head);
    masked_head = head & OS_LOG_RING_MASK;

    if (head & OS_LOG_RING_SIZE)
    {
        atomic_and(&os_log_ring.head, ~OS_LOG_RING_SIZE);
    }

    return masked_head;
}

static void log_ring_buffer_push_commit(void)
{
    atomic_increment(&os_log_ring.count);
}

static i32 log_ring_buffer_pop(void)
{
    i32 tail = 0;
    i32 masked_tail = 0;
    
    tail = atomic_increment(&os_log_ring.tail);
    masked_tail = tail & OS_LOG_RING_MASK;

    if (tail & OS_LOG_RING_SIZE)
    {
        atomic_and(&os_log_ring.tail, ~OS_LOG_RING_SIZE);
    }

    return masked_tail;
}

static void log_ring_buffer_pop_commit(void)
{
    atomic_decrement(&os_log_ring.count);
}

void os_log_set_level(OSLogLevel log_level)
{
    os_log_level = log_level;
}

void os_log(OSLogLevel log_level, const char* file, i32 line, const char* fmt, ...)
{
    if (default_output_file && log_level >= os_log_level)
    {
        i32 head = log_ring_buffer_push();
        {
            OSLogRingElement* element = os_log_ring.elements + head;
            va_list args;

            va_start(args, fmt);
            vsnprintf(element->fmt, OS_IO_MAX_OUTPUT_LENGTH, fmt, args);
            va_end(args);

            element->log_level = log_level;
            element->file = file;
            element->line = line;
        }
        log_ring_buffer_push_commit();
        os_thread_wake_by_address((void*)&os_log_ring.count);
    }
}

// TODO: Should we add multiple output?
void os_log_init(OSIOFile file)
{
    if (!default_output_file)
    {
        os_thread_create(&os_log_thread_procedure, 0);
        default_output_file = file;
    }
}
