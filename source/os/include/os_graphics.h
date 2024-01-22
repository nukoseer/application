#ifndef H_OS_GRAPHICS_H

typedef uptr OSGraphicsShader;
typedef uptr OSGraphicsInputLayout;

void os_graphics_use_shader(OSWindowHandle os_window_handle, OSGraphicsShader shader);
void os_graphics_use_input_layout(OSWindowHandle os_window_handle, OSGraphicsInputLayout input_layout);
void os_graphics_set_vertex_buffer_data(OSWindowHandle os_window_handle, const void* vertex_buffer_data, u32 vertex_buffer_size);
void os_graphics_add_vertex_buffer_data(OSWindowHandle os_window_handle, const void* vertex_buffer_data, u32 vertex_buffer_size);
OSGraphicsInputLayout os_graphics_create_input_layout(const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                                      const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count);
void os_graphics_create_texture(OSWindowHandle os_window_handle, const u32* texture_buffer, i32 width, i32 height);
OSGraphicsShader os_graphics_create_vertex_shader(const u8* shader_buffer, u32 shader_buffer_size);
OSGraphicsShader os_graphics_create_pixel_shader(const u8* shader_buffer, u32 shader_buffer_size);
void os_graphics_clear(OSWindowHandle os_window_handle, RGBA color);
void os_graphics_draw_rectangle(OSWindowHandle os_window_handle, i32 x, i32 y, i32 width, i32 height, RGBA color);
void os_graphics_draw_circle_section(OSWindowHandle os_window_handle, i32 center_x, i32 center_y, f32 radius,
                                     f32 start_angle, f32 end_angle, i32 segments, RGBA color);
void os_graphics_draw_triangle(OSWindowHandle os_window_handle, V2 v1, V2 v2, V2 v3, RGBA color);
void os_graphics_draw_circle(OSWindowHandle os_window_handle, i32 center_x, i32 center_y, f32 radius, RGBA color);
void os_graphics_draw_pixel(OSWindowHandle os_window_handle, i32 x, i32 y, RGBA color);
void os_graphics_draw(OSWindowHandle os_window_handle);

#define H_OS_GRAPHICS_H
#endif
