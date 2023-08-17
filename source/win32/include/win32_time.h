#ifndef H_WIN32_TIME_H

u64  win32_time_get_tick(void);
u64  win32_time_get_frequency(void);
void win32_time_sleep(u32 milliseconds);
void win32_time_system_now(OSDateTime* os_system_time);
void win32_time_local_now(OSDateTime* os_local_time);

#define H_WIN32_TIME_H
#endif
