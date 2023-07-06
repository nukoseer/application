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

static void application(void)
{
    OSWindowHandle os_window_handle = 0;
    OSWindowHandle os_window_handle2 = 0;
    OSRandomHandle os_random_handle = { 0 };
    i32 x = 0;
    i32 y = 0;
    i32 width = 0;
    i32 height = 0;

    os_init();
    os_window_handle = os_window_open("Application Window", 60, 60, 640, 480, FALSE);
    os_window_handle2 = os_window_open("Application Window2", 80, 80, 640, 480, TRUE);

    os_window_get_position(os_window_handle, &x, &y, &width, &height);
    os_window_set_position(os_window_handle2, x + width, y, width, height);

    os_log_set_level(OS_LOG_LEVEL_DEBUG);

    os_graphics_set_vertex_buffer_data(os_window_handle, vertices, sizeof(vertices) / 2);

    os_graphics_draw_rectangle(os_window_handle, 160, 120, 320, 240, Color(255.0f, 0.0f, 0.0f, 255.0f));
    os_graphics_draw_rectangle(os_window_handle2, 160, 120, 320, 240, Color(0.0f, 255.0f, 0.0f, 255.0f));

    os_graphics_add_vertex_buffer_data(os_window_handle2, (u8*)vertices + sizeof(vertices) / 2, sizeof(vertices) / 2);
    os_graphics_draw_triangle(os_window_handle,
                              Vec2(320.0f, 120.0f), Vec2(80.0f, 420.0f), Vec2(560.0f, 420.0f),
                              Color(210.0f, 39.0f, 210.0f, 128.0f));
    
    os_graphics_draw_circle(os_window_handle, width / 2, height / 2, 60, Color(0.0f, 0.0f, 255.0f, 255.0f));
    os_graphics_draw_circle_section(os_window_handle2, width / 2, height / 2, 60, 90.0f, 270.0f, 18, Color(200.0f, 66.0f, 115.0f, 255.0f));

    os_graphics_draw_pixel(os_window_handle, width / 2, height / 2, Color(255.0f, 255.0f, 255.0f, 255.0f));
    
    os_random_handle = os_random_init(44);
    OS_LOG_DEBUG("os_random_unilateral: %f", (f64)os_random_unilateral(os_random_handle));
    OS_LOG_DEBUG("os_random_bilateral: %f", (f64)os_random_bilateral(os_random_handle));

    {
        u32 texture_buffer0[] =
        {
            0x80000000, 0xFFFFFFFF,
            0xFFFFFFFF, 0x80000000,
        };
        
        u32 texture_buffer1[] =
        {
            0xFFFFFFFF, 0xFFFFFFFF,
            0xFFFFFFFF, 0xFFFFFFFF,
        };

        // TODO: Maybe, we should not associate textures with windows,
        // they are not directly related with windows but shaders?
        os_graphics_create_texture(os_window_handle, texture_buffer0, 2, 2);
        os_graphics_create_texture(os_window_handle, texture_buffer1, 2, 2);
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

        os_graphics_clear(os_window_handle, Color(100.0f, 149.0f, 237.0f, 255.0f));
        os_graphics_draw(os_window_handle);

        os_graphics_clear(os_window_handle2, Color(170.0f, 149.0f, 237.0f, 255.0f));
        os_graphics_draw(os_window_handle2);
    }

    os_destroy();
}

int main(void)
{
    run_test();
    application();
}
