#include "types.h"
#include "win32.h"
#include "win32_window.h"
#include "win32_io.h"

b32 win32_should_quit(void)
{
    return !win32_window_get_window_count();
}

void win32_destroy(void)
{
    win32_window_destroy();
}

void win32_init(void)
{
    win32_window_init();
    win32_io_init();
}
