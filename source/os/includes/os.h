#ifndef _H_OS_H_

typedef u64 OSHandle;

OSHandle os_open_window(const char* title, i32 width, i32 height);
void os_close_window(OSHandle os_handle);
OSEventList os_get_events(void);

#define _H_OS_H_
#endif
