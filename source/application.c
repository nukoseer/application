#include <stdio.h>
#include "test.h"
#include "os.h"

typedef struct Vertex
{
    f32 position[2];
    f32 uv[2];
    f32 color[3];
} Vertex;

static Vertex vertices[] =
{
    { { -0.00f, +0.75f }, { 25.0f, 50.0f }, { 1, 0, 0 } },
    { { +0.75f, -0.50f }, {  0.0f,  0.0f }, { 0, 1, 0 } },
    { { -0.75f, -0.50f }, { 50.0f,  0.0f }, { 0, 0, 1 } },  
};

static void application(void)
{
    OSWindowHandle os_window_handle = 0;
    // OSWindowHandle os_window_handle2 = 0;
    i32 x = 0;
    i32 y = 0;
    i32 width = 0;
    i32 height = 0;

    os_init();
    os_window_handle = os_window_open("Application Window", 30, 30, 640, 480, FALSE);
    // os_window_handle2 = os_window_open("Application Window2", 50, 50, 640, 480, TRUE);

    os_window_get_position(os_window_handle, &x, &y, &width, &height);
    // os_window_set_position(os_window_handle2, x + width, y, width, height);

    os_log_set_level(OS_LOG_LEVEL_DEBUG);

    os_graphics_set_vertex_buffer_data(os_window_handle, vertices, sizeof(vertices));

    {
        const char* input_layout_names[] = { "POSITION", "TEXCOORD", "COLOR" };
        u32 input_layout_offsets[] = { OFFSETOF(Vertex, position), OFFSETOF(Vertex, uv), OFFSETOF(Vertex, color) };
        u32 input_layout_formats[] = { 2, 2, 3 };

        os_graphics_set_vertex_input_layouts(os_window_handle,
                                             input_layout_names, input_layout_offsets, input_layout_formats,
                                             sizeof(Vertex), ARRAY_COUNT(input_layout_names));
    }
    
    while (!os_should_quit())
    {
        OSTimeTickHandle os_time_tick_handle = os_time_begin_tick();
        OSEventList event_list = os_window_get_events();

        os_time_sleep(16);

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
        // os_window_set_position(os_window_handle2, x + width, y, width, height);

        {
            char milliseconds_string[32] = { 0 };
            f64 milliseconds = os_time_end_tick(os_time_tick_handle);

            sprintf(milliseconds_string, "%.8f ms. %d fps.\n", milliseconds, (i32)(1000.0 / milliseconds));
            
            // os_window_set_title(os_window_handle, milliseconds_string);
            // os_window_set_title(os_window_handle2, milliseconds_string);
        }

        os_graphics_draw(os_window_handle);
    }

    os_destroy();
}

int main(void)
{
    run_test();
    application();
}
