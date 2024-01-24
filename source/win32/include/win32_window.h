#ifndef H_WIN32_WINDOW_H

struct OSEventList;
struct MemoryArena;

uptr win32_window_open(const char* title, i32 x, i32 y, i32 width, i32 height, b32 borderless);
b32  win32_window_close(uptr window_pointer);
uptr win32_window_get_from(uptr window_pointer);
uptr win32_window_get_window_from(uptr handle_pointer);
uptr win32_window_get_graphics_from(uptr window_pointer);
struct OSEventList win32_window_get_event_list(struct MemoryArena* arena);
b32  win32_window_get_position_and_size(uptr window_pointer, i32* x, i32* y, i32* width, i32* height);
b32  win32_window_set_position_and_size(uptr window_pointer, i32 x, i32 y, i32 width, i32 height);
b32  win32_window_set_title(uptr window_pointer, const char* title);
f32  win32_window_get_default_refresh_rate(void);
u32  win32_window_get_window_count(void);
void win32_window_destroy(void);
void win32_window_init(void);

#define H_WIN32_WINDOW_H
#endif
