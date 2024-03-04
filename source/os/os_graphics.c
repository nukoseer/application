#include "types.h"
#include "utils.h"
#include "memory_utils.h"
#include "maths.h"

#include "os_window.h"
#include "os_graphics.h"

typedef OSGraphicsBuffer   OSGraphicsCreateBuffer(OSGraphicsBufferDesc* buffer_desc);
typedef OSGraphicsSampler  OSGraphicsCreateSampler(OSGraphicsSamplerDesc* sampler_desc);
typedef OSGraphicsTexture  OSGraphicsCreateTexture(OSGraphicsTextureDesc* texture_desc);
typedef OSGraphicsShader   OSGraphicsCreateShader(OSGraphicsShaderDesc* shader_desc);
typedef OSGraphicsPipeline OSGraphicsCreatePipeline(OSGraphicsPipelineDesc* pipeline_desc);
typedef void               OSGraphicsApplyPipeline(OSGraphicsPipeline pipeline);
typedef void               OSGraphicsApplyBindings(OSGraphicsBindings* bindings);
typedef void               OSGraphicsApplyUniforms(u32 shader_stage_index, u32 uniform_index, const void* data, memory_size size);
typedef void               OSGraphicsDraw(uptr graphics_pointer, u32 index, u32 count);

typedef struct OSGraphicsTable
{
    OSGraphicsCreateBuffer* create_buffer;
    OSGraphicsCreateSampler* create_sampler;
    OSGraphicsCreateTexture* create_texture;
    OSGraphicsCreateShader* create_shader;
    OSGraphicsCreatePipeline* create_pipeline;
    OSGraphicsApplyPipeline* apply_pipeline;
    OSGraphicsApplyBindings* apply_bindings;
    OSGraphicsApplyUniforms* apply_uniforms;
    OSGraphicsDraw* draw;
} OSGraphicsTable;

#ifdef _WIN32
#include "win32_window.h"
#include "win32_graphics.h"

static OSGraphicsTable os_graphics_table =
{
    .create_buffer = &win32_graphics_create_buffer,
    .create_sampler = &win32_graphics_create_sampler,
    .create_texture = &win32_graphics_create_texture,
    .create_shader = &win32_graphics_create_shader,
    .create_pipeline = &win32_graphics_create_pipeline,
    .apply_pipeline = &win32_graphics_apply_pipeline,
    .apply_bindings = &win32_graphics_apply_bindings,
    .apply_uniforms = &win32_graphics_apply_uniforms,
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

#if 0
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

        // os_graphics_add_vertex_buffer_data(os_window, (const u8*)rectangle_buffer, sizeof(rectangle_buffer));
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
        
        // os_graphics_add_vertex_buffer_data(os_window, (const u8*)triangle_buffer, sizeof(triangle_buffer));
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
            // os_graphics_add_vertex_buffer_data(os_window, (u8*)circle_buffer, sizeof(circle_buffer));

            circle_buffer[0] = (f32)center_x + cos_f32(DEG_2_RAD * angle) * radius;
            circle_buffer[1] = (f32)center_y + sin_f32(DEG_2_RAD * angle) * radius;
            circle_buffer[2] = -1.0f;
            circle_buffer[3] = -1.0f;
            circle_buffer[4] = (f32)color.r;
            circle_buffer[5] = (f32)color.g;
            circle_buffer[6] = (f32)color.b;
            circle_buffer[7] = (f32)color.a;
            // os_graphics_add_vertex_buffer_data(os_window, (u8*)circle_buffer, sizeof(circle_buffer));

            circle_buffer[0] = (f32)center_x + cos_f32(DEG_2_RAD * (angle + steps)) * radius;
            circle_buffer[1] = (f32)center_y + sin_f32(DEG_2_RAD * (angle + steps)) * radius;
            circle_buffer[2] = -1.0f;
            circle_buffer[3] = -1.0f;
            circle_buffer[4] = (f32)color.r;
            circle_buffer[5] = (f32)color.g;
            circle_buffer[6] = (f32)color.b;
            circle_buffer[7] = (f32)color.a;
            // os_graphics_add_vertex_buffer_data(os_window, (u8*)circle_buffer, sizeof(circle_buffer));

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

        // os_graphics_add_vertex_buffer_data(os_window, (const u8*)pixel_buffer, sizeof(pixel_buffer));
    }
}
#endif

OSGraphicsBuffer os_graphics_create_buffer(OSGraphicsBufferDesc* buffer_desc)
{
    OSGraphicsBuffer os_graphics_buffer = 0;

    ASSERT(os_graphics_table.create_buffer);
    os_graphics_buffer = os_graphics_table.create_buffer(buffer_desc);

    return os_graphics_buffer;
}

OSGraphicsSampler os_graphics_create_sampler(OSGraphicsSamplerDesc* sampler_desc)
{
    OSGraphicsSampler os_graphics_sampler = 0;

    ASSERT(os_graphics_table.create_sampler);
    os_graphics_sampler = os_graphics_table.create_sampler(sampler_desc);

    return os_graphics_sampler;
}

OSGraphicsTexture os_graphics_create_texture(OSGraphicsTextureDesc* texture_desc)
{
    OSGraphicsTexture os_graphics_texture = 0;

    ASSERT(os_graphics_table.create_texture);
    os_graphics_texture = os_graphics_table.create_texture(texture_desc);

    return os_graphics_texture;
}

OSGraphicsShader os_graphics_create_shader(OSGraphicsShaderDesc* shader_desc)
{
    OSGraphicsShader os_graphics_shader = 0;

    ASSERT(os_graphics_table.create_shader);
    os_graphics_shader = os_graphics_table.create_shader(shader_desc);

    return os_graphics_shader;
}

OSGraphicsPipeline os_graphics_create_pipeline(OSGraphicsPipelineDesc* pipeline_desc)
{
    OSGraphicsPipeline os_graphics_pipeline = 0;

    ASSERT(os_graphics_table.create_pipeline);
    os_graphics_pipeline = os_graphics_table.create_pipeline(pipeline_desc);

    return os_graphics_pipeline;    
}

void os_graphics_apply_pipeline(OSGraphicsPipeline pipeline)
{
    ASSERT(os_graphics_table.apply_pipeline);
    os_graphics_table.apply_pipeline(pipeline);
}

void os_graphics_apply_bindings(OSGraphicsBindings* bindings)
{
    ASSERT(os_graphics_table.apply_bindings);
    os_graphics_table.apply_bindings(bindings);
}

void os_graphics_apply_uniforms(u32 shader_stage_index, u32 uniform_index, const void* data, memory_size size)
{
    ASSERT(os_graphics_table.apply_uniforms);
    os_graphics_table.apply_uniforms(shader_stage_index, uniform_index, data, size);
}

void os_graphics_draw(OSWindow os_window, u32 index, u32 count)
{
    uptr graphics = get_graphics_from_window(os_window);

    if (graphics)
    {
        ASSERT(os_graphics_table.draw);
        os_graphics_table.draw(graphics, index, count);
    }
}
