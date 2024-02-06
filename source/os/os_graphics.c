#include "types.h"
#include "utils.h"
#include "memory_utils.h"
#include "maths.h"

#include "os_window.h"
#include "os_graphics.h"

typedef void OSGraphicsUseShader(uptr graphics_pointer, OSGraphicsShader shader);
typedef void OSGraphicsUseInputLayout(uptr graphics_pointer, OSGraphicsInputLayout input_layout);
typedef void OSGraphicsSetVertexBufferData(uptr graphics_pointer, const void* vertex_buffer_data, u32 vertex_buffer_size);
typedef void OSGraphicsAddVertexBufferData(uptr graphics_pointer, const void* vertex_buffer_data, u32 vertex_buffer_size);
typedef OSGraphicsInputLayout OSGraphicsCreateInputLayout(const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                                          const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count);
typedef OSGraphicsTexture2D OSGraphicsCreateTexture2D(uptr graphics_pointer, const u32* texture_buffer, i32 width, i32 height);
typedef void OSGraphicsUseTexture2Ds(uptr graphics_pointer, uptr* texture_2Ds, u32 texture_count);
typedef OSGraphicsShader OSGraphicsCreateVertexShader(const u8* shader_buffer, u32 shader_buffer_size);
typedef OSGraphicsShader OSGraphicsCreatePixelShader(const u8* shader_buffer, u32 shader_buffer_size);
typedef void OSGraphicsClear(uptr graphics_pointer, RGBA color);
typedef void OSGraphicsDraw(uptr graphics_pointer);

typedef struct OSGraphicsTable
{
    OSGraphicsUseShader* use_shader;
    OSGraphicsUseInputLayout* use_input_layout;
    OSGraphicsSetVertexBufferData* set_vertex_buffer_data;
    OSGraphicsAddVertexBufferData* add_vertex_buffer_data;
    OSGraphicsCreateInputLayout* create_input_layout;
    OSGraphicsCreateTexture2D* create_texture_2D;
    OSGraphicsUseTexture2Ds* use_texture_2Ds;
    OSGraphicsCreateVertexShader* create_vertex_shader;
    OSGraphicsCreatePixelShader* create_pixel_shader;
    OSGraphicsClear* clear;
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
    .create_texture_2D = &win32_graphics_create_texture_2D,
    .use_texture_2Ds = &win32_graphics_use_texture_2Ds,
    .create_vertex_shader = &win32_graphics_create_vertex_shader,
    .create_pixel_shader = &win32_graphics_create_pixel_shader,
    .clear = &win32_graphics_clear,
    .draw = &win32_graphics_draw,
};

#else
#error _WIN32 must be defined.
#endif

// TODO: Are we sure this needs to be done in here instead of platform code?
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

OSGraphicsTexture2D os_graphics_create_texture_2D(OSWindow os_window, const u32* texture_buffer, i32 width, i32 height)
{
    uptr graphics = get_graphics_from_window(os_window);
    OSGraphicsTexture2D texture = 0;

    if (graphics)
    {
        ASSERT(os_graphics_table.create_texture_2D);
        texture = os_graphics_table.create_texture_2D(graphics, texture_buffer, width, height);
    }
    
    return texture;
}

void os_graphics_use_texture_2Ds(OSWindow os_window, OSGraphicsTexture2D* texture_2Ds, u32 texture_count)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.use_texture_2Ds);
        os_graphics_table.use_texture_2Ds(graphics, texture_2Ds, texture_count);
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
        f32 xf = (f32)x; f32 yf = (f32)y; f32 wf = (f32)width; f32 hf = (f32)height;
        // TODO: What to do with tex coords?
        const f32 rectangle_buffer[] = 
        {
            xf,      yf,      -1.0f, -1.0f, color.r, color.g, color.b, color.a,
            xf + wf, yf,      -1.0f, -1.0f, color.r, color.g, color.b, color.a,
            xf,      yf + hf, -1.0f, -1.0f, color.r, color.g, color.b, color.a,
            xf,      yf + hf, -1.0f, -1.0f, color.r, color.g, color.b, color.a,
            xf + wf, yf,      -1.0f, -1.0f, color.r, color.g, color.b, color.a,
            xf + wf, yf + hf, -1.0f, -1.0f, color.r, color.g, color.b, color.a,
        };

        os_graphics_add_vertex_buffer_data(os_window, (const u8*)rectangle_buffer, sizeof(rectangle_buffer));
    }
}

void os_graphics_draw_triangle(OSWindow os_window, V2 v1, V2 v2, V2 v3, RGBA color)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        const f32 triangle_buffer[] =
        {
            v1.x, v1.y, -1.0f, -1.0f, color.r, color.g, color.b, color.a,
            v2.x, v2.y, -1.0f, -1.0f, color.r, color.g, color.b, color.a,
            v3.x, v3.y, -1.0f, -1.0f, color.r, color.g, color.b, color.a,
        };
        
        os_graphics_add_vertex_buffer_data(os_window, (const u8*)triangle_buffer, sizeof(triangle_buffer));
    }
}

// TODO: We should test this function with different(unusual) angles.
void os_graphics_draw_circle_section(OSWindow os_window, i32 center_x, i32 center_y, f32 radius,
                                     f32 start_angle, f32 end_angle, i32 segments, RGBA color)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        f32 angle = 0;
        f32 steps = 0;
        i32 i = 0;
        f32 circle_buffer[8];
        i32 min_segments = (i32)ceil_f32((end_angle - start_angle) / 90.0f);

        if (end_angle < start_angle)
        {
            f32 temp;
            temp = start_angle;
            start_angle = end_angle;
            end_angle = temp;
        }

        if (segments < min_segments)
        {
            // NOTE: Source: https://stackoverflow.com/questions/2243668/when-drawing-an-ellipse-or-circle-with-opengl-how-many-vertices-should-we-use/2244088#2244088
            // and raylib
            // NOTE: Maximum angle between segments (error rate 0.5f)
            f32 th = acos_f32(2.0f * pow_f32(1.0f - (0.5f / radius), 2.0f) - 1.0f);
            segments = (i32)((end_angle - start_angle) * ceil_f32(2.0f * PI / th) / 360.0f);

            if (segments <= 0)
            {
                segments = min_segments;
            }
        }

        steps = (end_angle - start_angle) / (f32)segments;
        angle = start_angle;

        for (i = 0; i < segments; ++i)
        {
            circle_buffer[0] = (f32)center_x;
            circle_buffer[1] = (f32)center_y;
            circle_buffer[2] = -1.0f;
            circle_buffer[3] = -1.0f;
            circle_buffer[4] = (f32)color.r;
            circle_buffer[5] = (f32)color.g;
            circle_buffer[6] = (f32)color.b;
            circle_buffer[7] = (f32)color.a;
            os_graphics_add_vertex_buffer_data(os_window, (u8*)circle_buffer, sizeof(circle_buffer));

            circle_buffer[0] = (f32)center_x + cos_f32(DEG_2_RAD * angle) * radius;
            circle_buffer[1] = (f32)center_y + sin_f32(DEG_2_RAD * angle) * radius;
            circle_buffer[2] = -1.0f;
            circle_buffer[3] = -1.0f;
            circle_buffer[4] = (f32)color.r;
            circle_buffer[5] = (f32)color.g;
            circle_buffer[6] = (f32)color.b;
            circle_buffer[7] = (f32)color.a;
            os_graphics_add_vertex_buffer_data(os_window, (u8*)circle_buffer, sizeof(circle_buffer));

            circle_buffer[0] = (f32)center_x + cos_f32(DEG_2_RAD * (angle + steps)) * radius;
            circle_buffer[1] = (f32)center_y + sin_f32(DEG_2_RAD * (angle + steps)) * radius;
            circle_buffer[2] = -1.0f;
            circle_buffer[3] = -1.0f;
            circle_buffer[4] = (f32)color.r;
            circle_buffer[5] = (f32)color.g;
            circle_buffer[6] = (f32)color.b;
            circle_buffer[7] = (f32)color.a;
            os_graphics_add_vertex_buffer_data(os_window, (u8*)circle_buffer, sizeof(circle_buffer));

            angle += steps;
        }
    }
}

void os_graphics_draw_circle(OSWindow os_window, i32 center_x, i32 center_y, f32 radius, RGBA color)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        os_graphics_draw_circle_section(os_window, center_x, center_y, radius, 0.0f, 360.0f, 36, color);
    }
}

void os_graphics_draw_pixel(OSWindow os_window, i32 x, i32 y, RGBA color)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        f32 xf = (f32)x; f32 yf = (f32)y;
        // TODO: What to do with tex coords?
        const f32 pixel_buffer[] = 
        {
            xf,        yf,        -1.0f, -1.0f, color.r, color.g, color.b, color.a,
            xf + 1.0f, yf,        -1.0f, -1.0f, color.r, color.g, color.b, color.a,
            xf,        yf + 1.0f, -1.0f, -1.0f, color.r, color.g, color.b, color.a,
            xf,        yf + 1.0f, -1.0f, -1.0f, color.r, color.g, color.b, color.a,
            xf + 1.0f, yf,        -1.0f, -1.0f, color.r, color.g, color.b, color.a,
            xf + 1.0f, yf + 1.0f, -1.0f, -1.0f, color.r, color.g, color.b, color.a,
        };

        os_graphics_add_vertex_buffer_data(os_window, (const u8*)pixel_buffer, sizeof(pixel_buffer));
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
