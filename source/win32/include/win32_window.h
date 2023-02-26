#ifndef H_WIN32_WINDOW_H

struct OSEventList;
struct MemoryArena;

uptr win32_window_open(const char* title, i32 x, i32 y, i32 width, i32 height, b32 borderless);
b32  win32_window_close(uptr window_pointer);
uptr win32_window_get_handle_from(uptr window_pointer);
uptr win32_window_get_window_from(uptr handle_pointer);
uptr win32_window_get_graphics_handle_from(uptr window_pointer);
void win32_window_get_event_list(struct OSEventList* event_list, struct MemoryArena* event_arena);
b32  win32_window_get_position(uptr window_pointer, i32* x, i32* y, i32* width, i32* height);
b32  win32_window_set_position(uptr window_pointer, i32 x, i32 y, i32 width, i32 height);
b32  win32_window_set_title(uptr window_pointer, const char* title);
u32  win32_window_get_window_count(void);
void win32_window_destroy(void);
void win32_window_init(void);

#define H_WIN32_WINDOW_H
#endif
