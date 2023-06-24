#include "types.h"
#include "utils.h"

#include "os_window.h"
#include "os_graphics.h"

typedef void OSGraphicsSetVertexBufferData(uptr graphics_pointer, const void* vertex_buffer_data, u32 vertex_buffer_size);
typedef void OSGraphicsSetVertexInputLayouts(uptr graphics_pointer, const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                             const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count);
typedef void OSGraphicsCreateTexture(uptr graphics_pointer, const u32* texture_buffer, i32 width, i32 height);
typedef void OSGraphicsCreateVertexShader(uptr graphics_pointer, const u8* shader_buffer, u32 shader_buffer_size);
typedef void OSGraphicsCreatePixelShader(uptr graphics_pointer, const u8* shader_buffer, u32 shader_buffer_size);
typedef void OSGraphicsClear(uptr graphics_pointer, Color color);
typedef void OSGraphicsDrawRectangle(uptr graphics_pointer, i32 x, i32 y, i32 width, i32 height, Color color);
typedef void OSGraphicsDrawCircleSection(uptr graphics_pointer, i32 center_x, i32 center_y, f32 radius,
                                         f32 start_angle, f32 end_angle, i32 segments, Color color);
typedef void OSGraphicsDrawTriangle(uptr graphics_pointer, Vec2 v1, Vec2 v2, Vec2 v3, Color color);
typedef void OSGraphicsDrawCircle(uptr graphics_pointer, i32 center_x, i32 center_y, f32 radius, Color color);
typedef void OSGraphicsDrawPixel(uptr graphics_pointer, i32 x, i32 y, Color color);
typedef void OSGraphicsDraw(uptr graphics_pointer);

typedef struct OSGraphics
{
    OSGraphicsSetVertexBufferData* set_vertex_buffer_data;
    OSGraphicsSetVertexInputLayouts* set_vertex_input_layouts;
    OSGraphicsCreateTexture* create_texture;
    OSGraphicsCreateVertexShader* create_vertex_shader;
    OSGraphicsCreatePixelShader* create_pixel_shader;
    OSGraphicsClear* clear;
    OSGraphicsDrawRectangle* draw_rectangle;
    OSGraphicsDrawTriangle* draw_triangle;
    OSGraphicsDrawCircleSection* draw_circle_section;
    OSGraphicsDrawCircle* draw_circle;
    OSGraphicsDrawPixel* draw_pixel;
    OSGraphicsDraw* draw;
} OSGraphics;

#ifdef _WIN32
#include "win32_window.h"
#include "win32_graphics.h"

static OSGraphics os_graphics =
{
    .set_vertex_buffer_data = &win32_graphics_set_vertex_buffer_data,
    .set_vertex_input_layouts = &win32_graphics_set_vertex_input_layouts,
    .create_texture = &win32_graphics_create_texture,
    .create_vertex_shader = &win32_graphics_create_vertex_shader,
    .create_pixel_shader = &win32_graphics_create_pixel_shader,
    .clear = &win32_graphics_clear,
    .draw_rectangle = &win32_graphics_draw_rectangle,
    .draw_triangle = &win32_graphics_draw_triangle,
    .draw_circle_section = &win32_graphics_draw_circle_section,
    .draw_circle = &win32_graphics_draw_circle,
    .draw_pixel = &win32_graphics_draw_pixel,
    .draw = &win32_graphics_draw,
};

#else
#error _WIN32 must be defined.
#endif

static uptr get_graphics_handle_from_window(OSWindowHandle os_window_handle)
{
    uptr graphics_handle = os_window_get_graphics_handle(os_window_handle);

    ASSERT(graphics_handle);
    return graphics_handle;
}

void os_graphics_set_vertex_buffer_data(OSWindowHandle os_window_handle, const void* vertices, u32 vertex_count)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.set_vertex_buffer_data);
        os_graphics.set_vertex_buffer_data(graphics_handle, vertices, vertex_count);
    }
}

// TODO: So many parameters?
void os_graphics_set_vertex_input_layouts(OSWindowHandle os_window_handle, const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                          const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.set_vertex_input_layouts);
        os_graphics.set_vertex_input_layouts(graphics_handle, vertex_shader_buffer, vertex_shader_buffer_size,
                                             names, offsets, formats, stride, layout_count);
    }
}

void os_graphics_create_texture(OSWindowHandle os_window_handle, const u32* texture_buffer, i32 width, i32 height)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.create_texture);
        os_graphics.create_texture(graphics_handle, texture_buffer, width, height);
    }
}

void os_graphics_create_vertex_shader(OSWindowHandle os_window_handle, const u8* shader_buffer, u32 shader_buffer_size)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.create_vertex_shader);
        os_graphics.create_vertex_shader(graphics_handle, shader_buffer, shader_buffer_size);
    }
}

void os_graphics_create_pixel_shader(OSWindowHandle os_window_handle, const u8* shader_buffer, u32 shader_buffer_size)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.create_pixel_shader);
        os_graphics.create_pixel_shader(graphics_handle, shader_buffer, shader_buffer_size);
    }
}

void os_graphics_clear(OSWindowHandle os_window_handle, Color color)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.clear);
        os_graphics.clear(graphics_handle, color);
    }
}

void os_graphics_draw_rectangle(OSWindowHandle os_window_handle, i32 x, i32 y, i32 width, i32 height, Color color)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.draw_rectangle);
        os_graphics.draw_rectangle(graphics_handle, x, y, width, height, color);
    }
}

void os_graphics_draw_triangle(OSWindowHandle os_window_handle, Vec2 v1, Vec2 v2, Vec2 v3, Color color)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.draw_triangle);
        os_graphics.draw_triangle(graphics_handle, v1, v2, v3, color);
    }
}

void os_graphics_draw_circle_section(OSWindowHandle os_window_handle, i32 center_x, i32 center_y, f32 radius,
                                     f32 start_angle, f32 end_angle, i32 segments, Color color)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.draw_circle_section);
        os_graphics.draw_circle_section(graphics_handle, center_x, center_y, radius, start_angle, end_angle, segments, color);
    }
}

void os_graphics_draw_circle(OSWindowHandle os_window_handle, i32 center_x, i32 center_y, f32 radius, Color color)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.draw_circle);
        os_graphics.draw_circle(graphics_handle, center_x, center_y, radius, color);
    }
}

void os_graphics_draw_pixel(OSWindowHandle os_window_handle, i32 x, i32 y, Color color)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.draw_pixel);
        os_graphics.draw_pixel(graphics_handle, x, y, color);
    }
}

void os_graphics_draw(OSWindowHandle os_window_handle)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.draw);
        os_graphics.draw(graphics_handle);
    }
}
