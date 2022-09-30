#include "types.h"
#include "utils.h"
#include "mem.h"
#include "os.h"

typedef void OSInit(void);
typedef b32 OSQuit(void);
typedef void OSDestroy(void);

#ifdef _WIN32
#include "win32.h"

typedef struct OS
{
    OSQuit* should_quit;
    OSDestroy* destroy;
    OSInit* init;
} OS;

static OS os =
{
    .should_quit = &win32_should_quit,
    .destroy = &win32_destroy,
    .init = &win32_init,
};

#else
#error _WIN32 must be defined.
#endif

b32 os_should_quit(void)
{
    b32 quit = FALSE;

    ASSERT(os.should_quit);
    quit = os.should_quit();

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
    os_log_init();
}
