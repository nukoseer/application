#ifndef H_OS_GRAPHICS_H

void os_graphics_set_vertex_buffer_data(OSWindowHandle os_window_handle, void* vertices, u32 vertex_count);
void os_graphics_set_vertex_input_layouts(OSWindowHandle os_window_handle, const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                          const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count);
void os_graphics_create_texture(OSWindowHandle os_window_handle, const u32* texture_buffer, u32 width, u32 height);
void os_graphics_create_vertex_shader(OSWindowHandle os_window_handle, const u8* shader_buffer, u32 shader_buffer_size);
void os_graphics_create_pixel_shader(OSWindowHandle os_window_handle, const u8* shader_buffer, u32 shader_buffer_size);
void os_graphics_clear(uptr graphics, f32 r, f32 g, f32 b, f32 a);
void os_graphics_draw(OSWindowHandle os_window_handle);

#define H_OS_GRAPHICS_H
#endif
