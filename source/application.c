#include <stdio.h>
#include <stdlib.h>
#include "os.h"

static void stretchy_buffer_test(void)
{
    u32* arr = NULL;
    u32 i = 0;

    for (i = 0; i < 100; ++i)
    {
        BUFFER_PUSH(arr, i);
    }

    for (i = 0; i < BUFFER_LENGTH(arr); ++i)
    {
        ASSERT(i == arr[i]);
    }

    BUFFER_FREE(arr);
    ASSERT(NULL == arr);
}

static void application(void)
{
    b32 quit = FALSE;
    OSHandle os_handle = 0;

    os_init();
    os_handle = os_window_open("Application Window", 640, 480);

    while (!quit) //!os_quit()
    {
        OSTimerHandle os_timer_handle = os_timer_begin();
        OSEventList event_list = os_window_get_events();

        // sleep(16);

        for (OSEvent* event = event_list.first; event != 0; event = event->next)
        {
            if (event->type == OS_EVENT_TYPE_WINDOW_CLOSE)
            {
                os_window_close(os_handle);
                quit = TRUE;
                break;
            }
        }

        {
            f64 milliseconds = os_timer_end(os_timer_handle);
            (void)milliseconds;

            // printf("%.8f ms. %d fps.\n", milliseconds, (i32)(1000.0 / milliseconds));
        }
    }

    os_destroy();
}

int main(void)
{
    stretchy_buffer_test();
    application();
}
