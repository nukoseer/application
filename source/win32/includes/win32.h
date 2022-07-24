#ifndef H_WIN32_H

void* win32_window_open(const char* title, int width, int height);
void win32_window_close(void* window_pointer);
void* win32_get_handle_from_window( void* window_pointer);
void* win32_get_window_from_handle(void* handle_pointer);
void win32_get_event_list(OSEventList* event_list, MemoryArena* event_arena);
void win32_start(void);

#define H_WIN32_H
#endif
