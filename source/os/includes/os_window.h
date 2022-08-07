#ifndef H_OS_WINDOW_H

#include "os_events.h"

typedef memory_size OSHandle;

OSHandle os_window_open(const char* title, i32 width, i32 height);
void os_window_close(OSHandle os_handle);
OSEventList os_window_get_events(void);
void os_window_init(void);

#define H_OS_WINDOW_H
#endif
