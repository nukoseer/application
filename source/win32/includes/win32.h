#ifndef _H_WIN32_H_

void* win32_open_window(const char* title, int width, int height);
void win32_close_window(void* window_pointer);
void* win32_get_handle_from_window(const void* window_pointer);
void* win32_get_window_from_handle(const void* handle_pointer);
void win32_get_event_list(OSEventList* event_list, MemoryArena* event_arena);
void win32_start(void* instance_pointer);

#define _H_WIN32_H_
#endif
