#include "types.h"
#include "utils.h"

#include "os_window.h"
#include "os_graphics.h"

typedef OSGraphicsHandle OSGraphicsInit(OSWindowHandle os_handle_pointer);

typedef struct OSGraphics
{
    OSGraphicsInit* init;
} OSGraphics;

#ifdef _WIN32
#include "win32_graphics.h"

static OSGraphics os_graphics =
{
    .init = &win32_graphics_init,
};

#else
#error _WIN32 must be defined.
#endif

OSGraphicsHandle os_graphics_init(OSWindowHandle os_window_handle)
{
    OSGraphicsHandle os_graphics_handle = 0;

    ASSERT(os_graphics.init);
    os_graphics_handle = os_graphics.init(os_window_handle);
    ASSERT(os_graphics_handle);

    return os_graphics_handle;
}
