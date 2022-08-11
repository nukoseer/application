#ifndef H_WIN32_H

void* win32_window_open(const char* title, int width, int height);
void win32_window_close(void* window_pointer);
void* win32_window_get_handle_from(void* window_pointer);
void* win32_window_get_window_from(void* handle_pointer);
void win32_window_get_event_list(OSEventList* event_list, MemoryArena* event_arena);
b32 win32_should_quit(void);
void win32_destroy(void);
void win32_init(void);

#define H_WIN32_H
#endif
