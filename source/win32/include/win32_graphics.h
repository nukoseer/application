#ifndef H_WIN32_GRAPHICS_H

uptr win32_graphics_init(uptr handle_pointer);
void win32_graphics_set_vertex_buffer_data(uptr graphics, void* vertex_buffer_data, u32 vertex_buffer_size);
void win32_graphics_set_vertex_input_layouts(uptr graphics, const char** names, u32* offsets, u32* formats, u32 stride, u32 layout_count);
void win32_graphics_create_texture(uptr graphics_pointer);
void win32_graphics_clear(uptr graphics, f32 r, f32 g, f32 b, f32 a);
void win32_graphics_draw(uptr graphics);

#define H_WIN32_GRAPHICS_H
#endif
