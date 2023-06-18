#ifndef H_WIN32_GRAPHICS_H

uptr win32_graphics_init(uptr handle_pointer);
void win32_graphics_set_vertex_buffer_data(uptr graphics_pointer, const void* vertex_buffer_data, u32 vertex_buffer_size);
void win32_graphics_set_vertex_input_layouts(uptr graphics_pointer, const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                             const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count);
void win32_graphics_create_texture(uptr graphics_pointer, const u32* texture_buffer, i32 width, i32 height);
void win32_graphics_create_vertex_shader(uptr graphics_pointer, const u8* shader_buffer, u32 shader_buffer_size);
void win32_graphics_create_pixel_shader(uptr graphics_pointer, const u8* shader_buffer, u32 shader_buffer_size);
void win32_graphics_clear(uptr graphics_pointer, u8 r, u8 g, u8 b, u8 a);
void win32_graphics_draw_rectangle(uptr graphics_pointer, i32 x, i32 y, i32 width, i32 height, u8 r, u8 g, u8 b, u8 a);
void win32_graphics_draw_triangle(uptr graphics_pointer, f32 v1x, f32 v1y, f32 v2x, f32 v2y, f32 v3x, f32 v3y,
                                  u8 r, u8 g, u8 b, u8 a);
void win32_graphics_draw_circle(uptr graphics_pointer, i32 center_x, i32 center_y, f32 radius, u8 r, u8 g, u8 b, u8 a);
void win32_graphics_draw(uptr graphics_pointer);

#define H_WIN32_GRAPHICS_H
#endif
