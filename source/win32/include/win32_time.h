#ifndef H_WIN32_TIME_H

i64  win32_time_get_tick(void);
i64  win32_time_get_frequency(void);
void win32_time_sleep(u32 milliseconds);
void win32_time_now(OSSystemTime* os_system_time);

#define H_WIN32_TIME_H
#endif
