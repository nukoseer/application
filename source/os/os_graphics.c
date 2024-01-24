#include "types.h"
#include "utils.h"
#include "memory_utils.h"

#include "os_window.h"
#include "os_graphics.h"

typedef void OSGraphicsUseShader(uptr graphics_pointer, OSGraphicsShader shader);
typedef void OSGraphicsUseInputLayout(uptr graphics_pointer, OSGraphicsInputLayout input_layout);
typedef void OSGraphicsSetVertexBufferData(uptr graphics_pointer, const void* vertex_buffer_data, u32 vertex_buffer_size);
typedef void OSGraphicsAddVertexBufferData(uptr graphics_pointer, const void* vertex_buffer_data, u32 vertex_buffer_size);
typedef OSGraphicsInputLayout OSGraphicsCreateInputLayout(const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                                          const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count);
typedef void OSGraphicsCreateTexture(uptr graphics_pointer, const u32* texture_buffer, i32 width, i32 height);
typedef OSGraphicsShader OSGraphicsCreateVertexShader(const u8* shader_buffer, u32 shader_buffer_size);
typedef OSGraphicsShader OSGraphicsCreatePixelShader(const u8* shader_buffer, u32 shader_buffer_size);
typedef void OSGraphicsClear(uptr graphics_pointer, RGBA color);
typedef void OSGraphicsDrawRectangle(uptr graphics_pointer, i32 x, i32 y, i32 width, i32 height, RGBA color);
typedef void OSGraphicsDrawCircleSection(uptr graphics_pointer, i32 center_x, i32 center_y, f32 radius,
                                         f32 start_angle, f32 end_angle, i32 segments, RGBA color);
typedef void OSGraphicsDrawTriangle(uptr graphics_pointer, V2 v1, V2 v2, V2 v3, RGBA color);
typedef void OSGraphicsDrawCircle(uptr graphics_pointer, i32 center_x, i32 center_y, f32 radius, RGBA color);
typedef void OSGraphicsDrawPixel(uptr graphics_pointer, i32 x, i32 y, RGBA color);
typedef void OSGraphicsDraw(uptr graphics_pointer);

typedef struct OSGraphicsTable
{
    OSGraphicsUseShader* use_shader;
    OSGraphicsUseInputLayout* use_input_layout;
    OSGraphicsSetVertexBufferData* set_vertex_buffer_data;
    OSGraphicsAddVertexBufferData* add_vertex_buffer_data;
    OSGraphicsCreateInputLayout* create_input_layout;
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
} OSGraphicsTable;

#ifdef _WIN32
#include "win32_window.h"
#include "win32_graphics.h"

static OSGraphicsTable os_graphics_table =
{
    .use_shader = &win32_graphics_use_shader,
    .use_input_layout = &win32_graphics_use_input_layout,
    .set_vertex_buffer_data = &win32_graphics_set_vertex_buffer_data,
    .add_vertex_buffer_data = &win32_graphics_add_vertex_buffer_data,
    .create_input_layout = &win32_graphics_create_input_layout,
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

static uptr get_graphics_from_window(OSWindow os_window)
{
    uptr graphics = os_window_get_graphics(os_window);

    ASSERT(graphics);
    return graphics;
}

void os_graphics_use_shader(OSWindow os_window, OSGraphicsShader shader)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.use_shader);
        os_graphics_table.use_shader(graphics, shader);
    }
}

void os_graphics_use_input_layout(OSWindow os_window, OSGraphicsInputLayout input_layout)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.use_input_layout);
        os_graphics_table.use_input_layout(graphics, input_layout);
    }
}

void os_graphics_set_vertex_buffer_data(OSWindow os_window, const void* vertex_buffer_data, u32 vertex_buffer_size)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.set_vertex_buffer_data);
        os_graphics_table.set_vertex_buffer_data(graphics, vertex_buffer_data, vertex_buffer_size);
    }
}

void os_graphics_add_vertex_buffer_data(OSWindow os_window, const void* vertex_buffer_data, u32 vertex_buffer_size)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.add_vertex_buffer_data);
        os_graphics_table.add_vertex_buffer_data(graphics, vertex_buffer_data, vertex_buffer_size);
    }
}

// TODO: So many parameters?
OSGraphicsInputLayout os_graphics_create_input_layout(const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                                      const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count)
{
    OSGraphicsInputLayout os_input_layout = 0;
    
    ASSERT(os_graphics_table.create_input_layout);
    os_input_layout = os_graphics_table.create_input_layout(vertex_shader_buffer, vertex_shader_buffer_size,
                                                      names, offsets, formats, stride, layout_count);

    return os_input_layout;
}

void os_graphics_create_texture(OSWindow os_window, const u32* texture_buffer, i32 width, i32 height)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.create_texture);
        os_graphics_table.create_texture(graphics, texture_buffer, width, height);
    }
}

OSGraphicsShader os_graphics_create_vertex_shader(const u8* shader_buffer, u32 shader_buffer_size)
{
    OSGraphicsShader os_graphics_shader = 0;
    
    ASSERT(os_graphics_table.create_vertex_shader);
    os_graphics_shader = os_graphics_table.create_vertex_shader(shader_buffer, shader_buffer_size);

    return os_graphics_shader;
}

OSGraphicsShader os_graphics_create_pixel_shader(const u8* shader_buffer, u32 shader_buffer_size)
{
    OSGraphicsShader os_graphics_shader = 0;

    ASSERT(os_graphics_table.create_pixel_shader);
    os_graphics_shader = os_graphics_table.create_pixel_shader(shader_buffer, shader_buffer_size);

    return os_graphics_shader;
}

void os_graphics_clear(OSWindow os_window, RGBA color)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.clear);
        os_graphics_table.clear(graphics, color);
    }
}

void os_graphics_draw_rectangle(OSWindow os_window, i32 x, i32 y, i32 width, i32 height, RGBA color)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.draw_rectangle);
        os_graphics_table.draw_rectangle(graphics, x, y, width, height, color);
    }
}

void os_graphics_draw_triangle(OSWindow os_window, V2 v1, V2 v2, V2 v3, RGBA color)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.draw_triangle);
        os_graphics_table.draw_triangle(graphics, v1, v2, v3, color);
    }
}

void os_graphics_draw_circle_section(OSWindow os_window, i32 center_x, i32 center_y, f32 radius,
                                     f32 start_angle, f32 end_angle, i32 segments, RGBA color)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.draw_circle_section);
        os_graphics_table.draw_circle_section(graphics, center_x, center_y, radius, start_angle, end_angle, segments, color);
    }
}

void os_graphics_draw_circle(OSWindow os_window, i32 center_x, i32 center_y, f32 radius, RGBA color)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.draw_circle);
        os_graphics_table.draw_circle(graphics, center_x, center_y, radius, color);
    }
}

void os_graphics_draw_pixel(OSWindow os_window, i32 x, i32 y, RGBA color)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.draw_pixel);
        os_graphics_table.draw_pixel(graphics, x, y, color);
    }
}

void os_graphics_draw(OSWindow os_window)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.draw);
        os_graphics_table.draw(graphics);
    }
}
