#ifndef H_WIN32_TIMER_H

i64  win32_timer_get_tick(void);
i64  win32_timer_get_frequency(void);
void win32_timer_sleep(u32 milliseconds);

#define H_WIN32_TIMER_H
#endif
