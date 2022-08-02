#include "types.h"
#include "utils.h"
#include "memory.h"
#include "os_window.h"
#include "os.h"

typedef void OSInit(void);
typedef b32 OSQuit(void);
typedef void OSDestroy(void);

#ifdef WIN32
#include "win32.h"

typedef struct OS
{
    OSQuit* quit;
    OSDestroy* destroy;
    OSInit* init;
} OS;

static OS os =
{
    .quit = &win32_quit,
    .destroy = &win32_destroy,
    .init = &win32_init,
};

#else
#error WIN32 must be defined.
#endif

b32 os_quit(void)
{
    b32 quit = FALSE;

    ASSERT(os.quit);
    quit = os.quit();

    return quit;
}

void os_destroy(void)
{
    ASSERT(os.destroy);
    os.destroy();
}

void os_init(void)
{
    ASSERT(os.init);
    os.init();
    os_window_init();
}


