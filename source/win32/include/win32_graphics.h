#ifndef H_WIN32_GRAPHICS_H

typedef struct Win32Graphics Win32Graphics;

Win32Graphics* win32_graphics_init(uptr handle_pointer);
void win32_graphics_resize_swap_chain(Win32Graphics* graphics, i32 initial_width, i32 initial_height);

#define H_WIN32_GRAPHICS_H
#endif
