#ifndef H_OS_GRAPHICS_H

typedef uptr OSGraphicsShader;
typedef uptr OSGraphicsInputLayout;

void os_graphics_use_shader(OSWindow os_window, OSGraphicsShader shader);
void os_graphics_use_input_layout(OSWindow os_window, OSGraphicsInputLayout input_layout);
void os_graphics_set_vertex_buffer_data(OSWindow os_window, const void* vertex_buffer_data, u32 vertex_buffer_size);
void os_graphics_add_vertex_buffer_data(OSWindow os_window, const void* vertex_buffer_data, u32 vertex_buffer_size);
OSGraphicsInputLayout os_graphics_create_input_layout(const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                                      const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count);
void os_graphics_create_texture(OSWindow os_window, const u32* texture_buffer, i32 width, i32 height);
OSGraphicsShader os_graphics_create_vertex_shader(const u8* shader_buffer, u32 shader_buffer_size);
OSGraphicsShader os_graphics_create_pixel_shader(const u8* shader_buffer, u32 shader_buffer_size);
void os_graphics_clear(OSWindow os_window, RGBA color);
void os_graphics_draw_rectangle(OSWindow os_window, i32 x, i32 y, i32 width, i32 height, RGBA color);
void os_graphics_draw_circle_section(OSWindow os_window, i32 center_x, i32 center_y, f32 radius,
                                     f32 start_angle, f32 end_angle, i32 segments, RGBA color);
void os_graphics_draw_triangle(OSWindow os_window, V2 v1, V2 v2, V2 v3, RGBA color);
void os_graphics_draw_circle(OSWindow os_window, i32 center_x, i32 center_y, f32 radius, RGBA color);
void os_graphics_draw_pixel(OSWindow os_window, i32 x, i32 y, RGBA color);
void os_graphics_draw(OSWindow os_window);

#define H_OS_GRAPHICS_H
#endif
