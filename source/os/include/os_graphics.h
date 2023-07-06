#ifndef H_OS_GRAPHICS_H

void os_graphics_set_vertex_buffer_data(OSWindowHandle os_window_handle, const void* vertex_buffer_data, u32 vertex_buffer_size);
void os_graphics_add_vertex_buffer_data(OSWindowHandle os_window_handle, const void* vertex_buffer_data, u32 vertex_buffer_size);
void os_graphics_set_vertex_input_layouts(OSWindowHandle os_window_handle, const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                          const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count);
void os_graphics_create_texture(OSWindowHandle os_window_handle, const u32* texture_buffer, i32 width, i32 height);
void os_graphics_create_vertex_shader(OSWindowHandle os_window_handle, const u8* shader_buffer, u32 shader_buffer_size);
void os_graphics_create_pixel_shader(OSWindowHandle os_window_handle, const u8* shader_buffer, u32 shader_buffer_size);
void os_graphics_clear(OSWindowHandle os_window_handle, Color color);
void os_graphics_draw_rectangle(OSWindowHandle os_window_handle, i32 x, i32 y, i32 width, i32 height, Color color);
void os_graphics_draw_circle_section(OSWindowHandle os_window_handle, i32 center_x, i32 center_y, f32 radius,
                                     f32 start_angle, f32 end_angle, i32 segments, Color color);
void os_graphics_draw_triangle(OSWindowHandle os_window_handle, Vec2 v1, Vec2 v2, Vec2 v3, Color color);
void os_graphics_draw_circle(OSWindowHandle os_window_handle, i32 center_x, i32 center_y, f32 radius, Color color);
void os_graphics_draw_pixel(OSWindowHandle os_window_handle, i32 x, i32 y, Color color);
void os_graphics_draw(OSWindowHandle os_window_handle);

#define H_OS_GRAPHICS_H
#endif
