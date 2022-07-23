#include <Windows.h>
#include "types.h"
#include "memory.h"
#include "os_events.h"
#include "win32.h"

int WinMain(HINSTANCE instance,
            HINSTANCE prev_instance,
            LPSTR cmd_line,
            int show_cmd)
{
    (void)prev_instance; (void)cmd_line; (void)show_cmd;
    
    win32_start(instance);

    return 0;
}
