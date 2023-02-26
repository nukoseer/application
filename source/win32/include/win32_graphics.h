#ifndef H_WIN32_GRAPHICS_H

typedef struct Win32Graphics Win32Graphics;

Win32Graphics* win32_graphics_init(uptr handle_pointer);
void win32_graphics_resize_swap_chain(uptr graphics, i32 initial_width, i32 initial_height);
void win32_graphics_draw(uptr graphics);
void win32_graphics_set_vertex_buffer_data(uptr graphics, void* vertex_buffer_data, u32 vertex_buffer_size);
void win32_graphics_set_vertex_input_layouts(uptr graphics, const char** names, u32* offsets, u32* formats, u32 stride, u32 layout_count);

#define H_WIN32_GRAPHICS_H
#endif
