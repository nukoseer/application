#include <stdio.h>
#include "test.h"
#include "os.h"

// static f32 vertices[] =
// {
//      320.0f, 420.0f,     -1.0f, -1.0f,     255.0f, 0.0f,   0.0f,   255.0f,
//      560.0f, 120.0f,     -1.0f, -1.0f,     0.0f,   255.0f, 0.0f,   255.0f,
//      80.0f,  120.0f,     -1.0f, -1.0f,     0.0f,   0.0f,    255.0f, 255.0f,

//      320.0f, 120.0f,     -1.0f, -1.0f,     255.0f, 0.0f,   0.0f,   255.0f,
//      80.0f,  420.0f,     -1.0f, -1.0f,     0.0f,   255.0f, 0.0f,   255.0f,
//      560.0f, 420.0f,     -1.0f, -1.0f,     0.0f,   0.0f,    255.0f, 255.0f,
// };

static f32 vertices[] =
{
    320.0f, 420.0f,     25.0f, 50.0f,      255.0f, 0.0f,   0.0f,   255.0f,
    560.0f, 120.0f,     0.0f,  0.0f,       0.0f,   255.0f, 0.0f,   255.0f,
    80.0f,  120.0f,     50.0f, 0.0f,       0.0f,   0.0f,    255.0f, 255.0f,
    
    // 320.0f, 120.0f,     25.0f, 50.0f,      255.0f, 0.0f,   0.0f,   255.0f,
    // 80.0f,  420.0f,     0.0f,  0.0f,       0.0f,   255.0f, 0.0f,   255.0f,
    // 560.0f, 420.0f,     50.0f, 0.0f,       0.0f,   0.0f,    255.0f, 255.0f,
};

// static f32 vertices[] =
// {
//      -0.00f, +0.75f,     25.0f, 50.0f,    1.0f, 0.0f, 0.0f, 1.0f,
//      +0.75f, -0.50f,     0.0f,  0.0f,     0.0f, 1.0f, 0.0f, 1.0f,
//      -0.75f, -0.50f,     50.0f,  0.0f,    0.0f, 0.0f, 1.0f, 1.0f,

//      // -0.00f, -0.50f,     25.0f, 50.0f,    1.0f, 0.0f, 0.0f, 1.0f,
//      // -0.75f, +0.75f,     0.0f,  0.0f,     0.0f, 1.0f, 0.0f, 1.0f,
//      // +0.75f, +0.75f,     50.0f,  0.0f,    0.0f, 0.0f, 1.0f, 1.0f,
// };

// TODO: Probably we should get rid of the function tables in os_xx.c source files.
static void application(void)
{
    OSWindow os_window = 0;
    // Random random = { 0 };
    i32 x = 0;
    i32 y = 0;
    i32 width = 0;
    i32 height = 0;
    MemoryArena* frame_arena = allocate_memory_arena(KILOBYTES(5));

    os_init();
    os_window = os_window_open("Application", 60, 60, 640, 480, FALSE);
    os_window_get_position_and_size(os_window, &x, &y, &width, &height);

    TemporaryMemory shader_file_memory = begin_temporary_memory(frame_arena);
    const char* shader_file_names[] = { "d3d11_vertex_shader.o", "d3d11_pixel_shader.o" };
    OSIOFileContent vertex_shader_file = os_io_file_read_by_name(shader_file_memory.arena, shader_file_names[0]);
    OSIOFileContent pixel_shader_file = os_io_file_read_by_name(shader_file_memory.arena, shader_file_names[1]);

    OSGraphicsShader shader = os_graphics_create_shader(&(OSGraphicsShaderDesc) {
            .attributes = { [0].semantic_name="POSITION", [1].semantic_name="TEXCOORD", [2].semantic_name="COLOR" },
            .vertex_shader = {
                .byte_code = (const char*)vertex_shader_file.data,
                .byte_code_size = vertex_shader_file.size,
                .uniform_blocks[0].uniforms = {
                    [0] = { .layout_type = OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_FLOAT4, .array_count = 1 },
                },
                .uniform_blocks[0].size = 4 * sizeof(f32),
            },
            .pixel_shader = { .byte_code = (const char*)pixel_shader_file.data, .byte_code_size = pixel_shader_file.size },
        });

    OSGraphicsBuffer vertex_buffer = os_graphics_create_buffer(&(OSGraphicsBufferDesc) {
            .type = OS_GRAPHICS_BUFFER_TYPE_VERTEX_BUFFER,
            .usage = OS_GRAPHICS_USAGE_TYPE_IMMUTABLE,
            .data = vertices,
            .size = sizeof(vertices),
        });

    OSGraphicsPipeline pipeline = os_graphics_create_pipeline(&(OSGraphicsPipelineDesc) {
            .shader = shader,
            .vertex_layout = {
                .attributes = {
                    [0] = { .offset = 0,                                     .format = OS_GRAPHICS_VERTEX_FORMAT_TYPE_FLOAT2 },
                    [1] = { .offset = 2 * sizeof(f32),                       .format = OS_GRAPHICS_VERTEX_FORMAT_TYPE_FLOAT2 },
                    [2] = { .offset = (2 * sizeof(f32)) + (2 * sizeof(f32)), .format = OS_GRAPHICS_VERTEX_FORMAT_TYPE_FLOAT4 },
                },
                .vertex_buffer_layouts = {
                    [0].stride = (2 * sizeof(f32)) + (2 * sizeof(f32)) + (4 * sizeof(f32)),
                },
            },
            .primitive_type = OS_GRAPHICS_PRIMITIVE_TYPE_TRIANGLES,
        });

    u32 texture_buffer[] =
    {
        0x80000000, 0xFFFFFFFF,
        0xFFFFFFFF, 0x80000000,
    };

    OSGraphicsTexture texture = os_graphics_create_texture(&(OSGraphicsTextureDesc) {
            .type = OS_GRAPHICS_TEXTURE_TYPE_2D,
            .usage = OS_GRAPHICS_USAGE_TYPE_DEFAULT,
            .format = OS_GRAPHICS_PIXEL_FORMAT_R8G8B8A8,
            .width = 2,
            .height = 2,
            .data = texture_buffer,
            .size = sizeof(texture_buffer),
        });

    OSGraphicsSampler sampler = os_graphics_create_sampler(&(OSGraphicsSamplerDesc) { 0 });

    OSGraphicsBindings bindings =
    {
        .pixel_shader = { .textures[0] = texture, .samplers[0] = sampler },
        .vertex_buffers[0] = vertex_buffer,
    };

    f32 size[] = { (f32)width, (f32)height, 0.0f, 0.0f };

    end_temporary_memory(shader_file_memory);

    // OSIOFile default_console = os_io_console_init();
    // os_log_init(default_console); 
    // os_log_set_level(OS_LOG_LEVEL_TRACE);

    // os_graphics_set_vertex_buffer_data(os_window, vertices, sizeof(vertices) / 2);
    // os_graphics_draw_rectangle(os_window, 160, 120, 320, 240, RGBA(255.0f, 0.0f, 0.0f, 255.0f));
    // os_graphics_draw_triangle(os_window,
    //                           V2(320.0f, 120.0f), V2(80.0f, 420.0f), V2(560.0f, 420.0f),
    //                           RGBA(210.0f, 39.0f, 210.0f, 128.0f));
    // os_graphics_draw_circle(os_window, width / 2, height / 2, 60, RGBA(0.0f, 0.0f, 255.0f, 255.0f));
    // os_graphics_draw_circle_section(os_window, width / 2, height / 2, 60, 90.0f, 270.0f, 18, RGBA(200.0f, 66.0f, 115.0f, 255.0f));
    // os_graphics_draw_pixel(os_window, width / 2, height / 2, RGBA(255.0f, 255.0f, 255.0f, 255.0f));
    
    // random = random_init(44);
    // OS_LOG_DEBUG("random_unilateral: %f", (f64)random_unilateral(random));
    // OS_LOG_DEBUG("random_bilateral: %f", (f64)random_bilateral(random));
    
    f32 default_refresh_rate = 60.0f; // os_window_get_default_refresh_rate();
    f32 target_seconds_per_frame = 1.0f / default_refresh_rate;
    u64 last_tick = os_time_get_tick();
    while (!os_should_quit())
    {
        TemporaryMemory frame_memory = begin_temporary_memory(frame_arena);
        OSEventList event_list = os_window_get_events(frame_memory.arena);

        os_graphics_apply_pipeline(pipeline);
        os_graphics_apply_bindings(&bindings);
        os_graphics_apply_uniforms(OS_GRAPHICS_VERTEX_SHADER_STAGE, 0, size, sizeof(size));

        // TODO: Still not sure how to handle input events.
        FOREACH(event_list.first, OSEvent, event)
        {
            if (event->type == OS_EVENT_TYPE_WINDOW_CLOSE)
            {
                OS_LOG_DEBUG("OS_EVENT_TYPE_WINDOW_CLOSE");
                os_window_close(event->window);
            }
            else if (event->type == OS_EVENT_TYPE_PRESS)
            {
                OS_LOG_TRACE("handle: %llu, key: %d", event->window, event->key);
            }
        }

        os_graphics_draw(os_window, 0, 3);

        {
            u64 tick = os_time_get_tick();
            f32 elapsed_seconds = (f32)os_time_get_elapsed_seconds(last_tick, tick);

            if (elapsed_seconds < target_seconds_per_frame)
            {
                u32 sleep_ms = (u32)(1000.0f * (target_seconds_per_frame - elapsed_seconds));
                os_time_sleep(sleep_ms);
            }

            char milliseconds_string[32] = { 0 };
            f64 milliseconds = os_time_get_elapsed_seconds(last_tick, os_time_get_tick()) * 1000.0;
            sprintf(milliseconds_string, "%.2f ms. %.2f fps\n",  milliseconds, 1000.0 / milliseconds);
            os_window_set_title(os_window, milliseconds_string);
        }

        last_tick = os_time_get_tick();
        end_temporary_memory(frame_memory);
    }

    os_destroy();
}

int main(void)
{
    run_test();
    application();
}
