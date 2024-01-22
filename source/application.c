#include <stdio.h>
#include "test.h"
#include "os.h"

static f32 vertices[] =
{
     320.0f, 420.0f,     -1.0f, -1.0f,     255.0f, 0.0f,   0.0f,   255.0f,
     560.0f, 120.0f,     -1.0f, -1.0f,     0.0f,   255.0f, 0.0f,   255.0f,
     80.0f,  120.0f,     -1.0f, -1.0f,     0.0f,   0.0f,    255.0f, 255.0f,

     320.0f, 120.0f,     -1.0f, -1.0f,     255.0f, 0.0f,   0.0f,   255.0f,
     80.0f,  420.0f,     -1.0f, -1.0f,     0.0f,   255.0f, 0.0f,   255.0f,
     560.0f, 420.0f,     -1.0f, -1.0f,     0.0f,   0.0f,    255.0f, 255.0f,
};

// static f32 vertices[] =
// {
//      -0.00f, +0.75f,     25.0f, 50.0f,    1.0f, 0.0f, 0.0f, 1.0f,
//      +0.75f, -0.50f,     0.0f,  0.0f,     0.0f, 1.0f, 0.0f, 1.0f,
//      -0.75f, -0.50f,     50.0f,  0.0f,    0.0f, 0.0f, 1.0f, 1.0f,

//      -0.00f, -0.50f,     25.0f, 50.0f,    1.0f, 0.0f, 0.0f, 1.0f,
//      -0.75f, +0.75f,     0.0f,  0.0f,     0.0f, 1.0f, 0.0f, 1.0f,
//      +0.75f, +0.75f,     50.0f,  0.0f,    0.0f, 0.0f, 1.0f, 1.0f,
// };

// TODO: Is it a good idea to have a default shader? We store
// everything about graphics in Win32Graphics struct but probably it
// is not good idea. If we want to create different shaders how we
// should do it?  If we create different shaders, we can't have
// standart way to draw vertices because we won't know data layout of
// vertex shader, so we must also write special draw functions for
// every shader with different data layout?  It means user should
// write draw functions instead of having standart functions in
// os/win32 layer.  This shows to me that maybe it is better to leave
// it like as it is (just standart basic shader) because our purpose
// is not to make so sophistacated renderer. If we can handle simple
// 2D graphics it should be enough for us.

// TODO: Our default shader? Not sure about TEXCOORD, uv.
static void graphics_init(OSWindowHandle os_window_handle)
{
    const char* shader_file_names[] = { "d3d11_vertex_shader.o", "d3d11_pixel_shader.o" };
    const char* input_layout_names[] = { "POSITION", "TEXCOORD", "COLOR" };
    struct Vertex { f32 position[2]; f32 uv[2]; f32 color[4]; };
    u32 input_layout_offsets[] = { OFFSETOF(struct Vertex, position), OFFSETOF(struct Vertex, uv), OFFSETOF(struct Vertex, color) };
    u32 input_layout_formats[] = { 2, 2, 4 };
    OSIOFileHandle vertex_shader_file = os_io_file_open(shader_file_names[0], 1); // NOTE: Read mode
    u32 vertex_shader_buffer_size = os_io_file_size(vertex_shader_file);
    u8* vertex_shader_buffer = os_memory_heap_allocate(vertex_shader_buffer_size, FALSE);
    OSIOFileHandle pixel_shader_file = os_io_file_open(shader_file_names[1], 1); // NOTE: Read mode
    u32 pixel_shader_buffer_size = os_io_file_size(pixel_shader_file);
    u8* pixel_shader_buffer = os_memory_heap_allocate(pixel_shader_buffer_size, FALSE);

    // TODO: Check size of read bytes?
    os_io_file_read(vertex_shader_file, (char*)vertex_shader_buffer, vertex_shader_buffer_size);
    os_io_file_read(pixel_shader_file, (char*)pixel_shader_buffer, pixel_shader_buffer_size);

    OSGraphicsShader vertex_shader = os_graphics_create_vertex_shader(vertex_shader_buffer, vertex_shader_buffer_size);
    OSGraphicsShader pixel_shader = os_graphics_create_pixel_shader(pixel_shader_buffer, pixel_shader_buffer_size);

    OSGraphicsInputLayout input_layout = os_graphics_create_input_layout(vertex_shader_buffer,
                                                                         vertex_shader_buffer_size,
                                                                         input_layout_names,
                                                                         input_layout_offsets,
                                                                         input_layout_formats,
                                                                         sizeof(struct Vertex), ARRAY_COUNT(input_layout_names));

    os_graphics_use_shader(os_window_handle, vertex_shader);
    os_graphics_use_shader(os_window_handle, pixel_shader);
    os_graphics_use_input_layout(os_window_handle, input_layout);

    os_memory_heap_release(vertex_shader_buffer);
    os_memory_heap_release(pixel_shader_buffer);
    os_io_file_close(vertex_shader_file);
    os_io_file_close(pixel_shader_file);
}

static void application(void)
{
    OSWindowHandle os_window_handle = 0;
    RandomHandle random_handle = { 0 };
    i32 x = 0;
    i32 y = 0;
    i32 width = 0;
    i32 height = 0;

    os_init();
    os_window_handle = os_window_open("Application", 60, 60, 640, 480, FALSE);

    graphics_init(os_window_handle);
    
    os_window_get_position_and_size(os_window_handle, &x, &y, &width, &height);

    os_log_set_level(OS_LOG_LEVEL_TRACE);

    os_graphics_set_vertex_buffer_data(os_window_handle, vertices, sizeof(vertices) / 2);
    os_graphics_draw_rectangle(os_window_handle, 160, 120, 320, 240, RGBA(255.0f, 0.0f, 0.0f, 255.0f));
    os_graphics_draw_triangle(os_window_handle,
                              V2(320.0f, 120.0f), V2(80.0f, 420.0f), V2(560.0f, 420.0f),
                              RGBA(210.0f, 39.0f, 210.0f, 128.0f));
    os_graphics_draw_circle(os_window_handle, width / 2, height / 2, 60, RGBA(0.0f, 0.0f, 255.0f, 255.0f));
    os_graphics_draw_circle_section(os_window_handle, width / 2, height / 2, 60, 90.0f, 270.0f, 18, RGBA(200.0f, 66.0f, 115.0f, 255.0f));
    os_graphics_draw_pixel(os_window_handle, width / 2, height / 2, RGBA(255.0f, 255.0f, 255.0f, 255.0f));
    
    random_handle = random_init(44);
    OS_LOG_DEBUG("random_unilateral: %f", (f64)random_unilateral(random_handle));
    OS_LOG_DEBUG("random_bilateral: %f", (f64)random_bilateral(random_handle));

    // {
    //     u32 texture_buffer0[] =
    //     {
    //         0x80000000, 0xFFFFFFFF,
    //         0xFFFFFFFF, 0x80000000,
    //     };
        
    //     u32 texture_buffer1[] =
    //     {
    //         0xFFFFFFFF, 0xFFFFFFFF,
    //         0xFFFFFFFF, 0xFFFFFFFF,
    //     };

    //     // TODO: Maybe, we should not associate textures with windows,
    //     // they are not directly related with windows but shaders?
    //     os_graphics_create_texture(os_window_handle, texture_buffer0, 2, 2);
    //     os_graphics_create_texture(os_window_handle, texture_buffer1, 2, 2);
    // }

    MemoryArena* frame_arena = allocate_memory_arena(KILOBYTES(2));

    while (!os_should_quit())
    {
        TemporaryMemory frame_memory = begin_temporary_memory(frame_arena);
        OSTimeTickHandle os_time_tick_handle = os_time_begin_tick();
        OSEventList event_list = os_window_get_events(frame_memory.arena);

        os_time_sleep(16);

        // TODO: Still not sure how to handle input events.
        for (OSEvent* event = event_list.first; event != 0; event = event->next)
        {
            if (event->type == OS_EVENT_TYPE_WINDOW_CLOSE)
            {
                OS_LOG_DEBUG("OS_EVENT_TYPE_WINDOW_CLOSE");
                os_window_close(event->window_handle);
            }
            else if (event->type == OS_EVENT_TYPE_PRESS)
            {
                OS_LOG_TRACE("handle: %llu, key: %d", event->window_handle, event->key);
            }
        }

        // os_window_get_position(os_window_handle, &x, &y, &width, &height);

        {
            char milliseconds_string[32] = { 0 };
            u64 tick = os_time_end_tick(os_time_tick_handle);
            f64 milliseconds = os_time_tick_to_milliseconds(tick);

            sprintf(milliseconds_string, "%.2f ms. %.2f fps.\n",  milliseconds, 1000.0 / milliseconds);

            // os_window_set_title(os_window_handle, milliseconds_string);
        }

        os_graphics_clear(os_window_handle, RGBA(100.0f, 149.0f, 237.0f, 255.0f));
        os_graphics_draw(os_window_handle);

        end_temporary_memory(frame_memory);
    }

    os_destroy();
}

int main(void)
{
    run_test();
    application();
}
