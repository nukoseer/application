#ifndef H_WIN32_H

void* win32_window_open(const char* title, int width, int height);
void win32_window_close(void* window_pointer);
void* win32_get_handle_from_window(void* window_pointer);
void* win32_get_window_from_handle(void* handle_pointer);
void win32_window_get_event_list(OSEventList* event_list, MemoryArena* event_arena);
b32 win32_quit(void);
void win32_destroy(void);
void win32_init(void);
// void win32_start(void);

#define H_WIN32_H
#endif
