#ifndef H_WIN32_WINDOW_H

struct OSEventList;
struct MemoryArena;

void* win32_window_open(const char* title, int width, int height);
void win32_window_close(void* window_pointer);
void* win32_window_get_handle_from(void* window_pointer);
void* win32_window_get_window_from(void* handle_pointer);
void win32_window_get_event_list(struct OSEventList* event_list, struct MemoryArena* event_arena);
u32 win32_window_get_window_count(void);
void win32_window_destroy(void);
void win32_window_init(void);

#define H_WIN32_WINDOW_H
#endif
