#ifndef H_WIN32_TIME_H

i64  win32_time_get_tick(void);
i64  win32_time_get_frequency(void);
void win32_time_sleep(u32 milliseconds);

#define H_WIN32_TIME_H
#endif
