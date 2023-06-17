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
typedef void OSGraphicsClear(uptr graphics_pointer, u8 r, u8 g, u8 b, u8 a);
typedef void OSGraphicsDrawRectangle(uptr graphics_pointer, i32 x, i32 y, i32 width, i32 height, u8 r, u8 g, u8 b, u8 a);
typedef void OSGraphicsDrawTriangle(uptr graphics_pointer, f32 v1x, f32 v1y, f32 v2x, f32 v2y, f32 v3x, f32 v3y,
                                    u8 r, u8 g, u8 b, u8 a);
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

void os_graphics_clear(OSWindowHandle os_window_handle, u8 r, u8 g, u8 b, u8 a)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.clear);
        os_graphics.clear(graphics_handle, r, g, b, a);
    }
}

void os_graphics_draw_rectangle(OSWindowHandle os_window_handle, i32 x, i32 y, i32 width, i32 height, u8 r, u8 g, u8 b, u8 a)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.draw_rectangle);
        os_graphics.draw_rectangle(graphics_handle, x, y, width, height, r, g, b, a);
    }    
}

void os_graphics_draw_triangle(OSWindowHandle os_window_handle, f32 v1x, f32 v1y, f32 v2x, f32 v2y, f32 v3x, f32 v3y,
                               u8 r, u8 g, u8 b, u8 a)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    if (graphics_handle)
    {
        ASSERT(os_graphics.draw_triangle);
        os_graphics.draw_triangle(graphics_handle, v1x, v1y, v2x, v2y, v3x, v3y, r, g, b, a);
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
