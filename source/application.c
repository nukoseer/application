#include <stdio.h>
#include "os.h"
#include "test.h"

static void application(void)
{
    OSWindowHandle os_window_handle = 0;
    OSWindowHandle os_window_handle2 = 0;
    i32 x = 0;
    i32 y = 0;
    i32 width = 0;
    i32 height = 0;

    os_init();
    os_window_handle = os_window_open("Application Window", 640, 480);
    os_window_handle2 = os_window_open("Application Window2", 640, 480);

    os_window_get_position(os_window_handle, &x, &y, &width, &height);
    os_window_set_position(os_window_handle2, x + width, y, width, height);

    while (!os_should_quit())
    {
        OSTimeTickHandle os_time_tick_handle = os_time_begin_tick();
        OSEventList event_list = os_window_get_events();

        os_time_sleep(16);

        for (OSEvent* event = event_list.first; event != 0; event = event->next)
        {
            if (event->type == OS_EVENT_TYPE_WINDOW_CLOSE)
            {
                os_io_write_console("handle: %llu, OS_EVENT_TYPE_WINDOW_CLOSE\n", event->window_handle);
                os_window_close(event->window_handle);
            }
            else if (event->type == OS_EVENT_TYPE_PRESS)
            {
                os_io_set_console_fg_color(event->key % 4);
                // os_io_write_console("handle: %llu, key: %d\n", event->window_handle, event->key);
                os_io_log_trace("handle: %llu, key: %d\n", event->window_handle, event->key);
            }
        }
        // os_window_get_position(os_window_handle, &x, &y, &width, &height);
        // os_window_set_position(os_window_handle2, x + width, y, width, height);

        {
            char milliseconds_string[32] = { 0 };
            f64 milliseconds = os_time_end_tick(os_time_tick_handle);
            // UNUSED_VARIABLE(milliseconds);

            sprintf(milliseconds_string, "%.8f ms. %d fps.\n", milliseconds, (i32)(1000.0 / milliseconds));
            // os_io_write_console(milliseconds_string);
            
            // os_window_set_title(os_window_handle, milliseconds_string);
            // os_window_set_title(os_window_handle2, milliseconds_string);
        }
    }

    os_destroy();
}

int main(void)
{
    run_test();
    application();
}
