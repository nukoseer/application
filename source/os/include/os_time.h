#ifndef H_OS_TIME_H

typedef struct OSDateTime
{
    i32 year;
    i32 month;
    i32 day_of_week;
    i32 day;
    i32 hour;
    i32 minute;
    i32 second;
    i32 milliseconds;
} OSDateTime;

typedef u32 OSTimeTickHandle;

u64 os_time_get_tick(void);
u64 os_time_get_frequency(void);
f64 os_time_tick_to_milliseconds(u64 tick);
OSTimeTickHandle os_time_begin_tick(void);
u64 os_time_end_tick(OSTimeTickHandle os_time_tick_handle);
void os_time_sleep(u32 milliseconds);
OSDateTime os_time_system_now(void);
OSDateTime os_time_local_now(void);


#define H_OS_TIME_H
#endif

    
