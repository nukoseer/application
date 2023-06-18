#ifndef H_OS_GRAPHICS_H

void os_graphics_set_vertex_buffer_data(OSWindowHandle os_window_handle, const void* vertices, u32 vertex_count);
void os_graphics_set_vertex_input_layouts(OSWindowHandle os_window_handle, const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                          const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count);
void os_graphics_create_texture(OSWindowHandle os_window_handle, const u32* texture_buffer, i32 width, i32 height);
void os_graphics_create_vertex_shader(OSWindowHandle os_window_handle, const u8* shader_buffer, u32 shader_buffer_size);
void os_graphics_create_pixel_shader(OSWindowHandle os_window_handle, const u8* shader_buffer, u32 shader_buffer_size);
void os_graphics_clear(OSWindowHandle os_window_handle, u8 r, u8 g, u8 b, u8 a);
void os_graphics_draw_rectangle(OSWindowHandle os_window_handle, i32 x, i32 y, i32 width, i32 height, u8 r, u8 g, u8 b, u8 a);
void os_graphics_draw_triangle(OSWindowHandle os_window_handle, f32 v1x, f32 v1y, f32 v2x, f32 v2y, f32 v3x, f32 v3y,
                               u8 r, u8 g, u8 b, u8 a);
void os_graphics_draw_circle(OSWindowHandle os_window_handle, i32 center_x, i32 center_y, f32 radius, u8 r, u8 g, u8 b, u8 a);
void os_graphics_draw(OSWindowHandle os_window_handle);

#define H_OS_GRAPHICS_H
#endif
