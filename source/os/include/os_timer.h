#ifndef H_OS_TIMER_H

typedef u32 OSTimerHandle;

i64 os_timer_get_tick(void);
i64 os_timer_get_frequency(void);
f64 os_timer_ticks_to_milliseconds(i64 tick);
OSTimerHandle os_timer_begin(void);
f64 os_timer_end(OSTimerHandle os_timer_handle);

#define H_OS_TIMER_H
#endif

    
