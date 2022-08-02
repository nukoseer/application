#include <stdio.h>
#include <stdlib.h>
#include "os.h"

int main(void)
{
    b32 quit = FALSE;
    OSHandle os_handle = 0;
    u32* arr = 0;
    u32 i = 0;

    // BUFFER_PUSH(arr, 5);
    // BUFFER_PUSH(arr, 6);

    for (i = 0; i < 100; ++i)
    {
        BUFFER_PUSH(arr, i);
        printf("%d\n", arr[i]);
    }

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

    BUFFER_FREE(arr);
    os_destroy();
}
