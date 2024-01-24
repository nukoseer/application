#include "types.h"
#include "utils.h"
#include "memory_utils.h"
#include "os.h"

typedef void OSInit(void);
typedef b32 OSQuit(void);
typedef void OSDestroy(void);
typedef OSModuleHandle OSLoadLibrary(const char* module_name);
typedef OSProcedureAddress OSGetProcedureAddress(OSModuleHandle module_handle, const char* procedure_name);

#ifdef _WIN32
#include "win32.h"

typedef struct OS
{
    OSGetProcedureAddress* get_procedure_address;
    OSLoadLibrary* load_library;
    OSQuit* should_quit;
    OSDestroy* destroy;
    OSInit* init;
} OS;

static OS os =
{
    .get_procedure_address = &win32_get_procedure_address,
    .load_library = &win32_load_library,
    .should_quit = &win32_should_quit,
    .destroy = &win32_destroy,
    .init = &win32_init,
};

#else
#error _WIN32 must be defined.
#endif

OSProcedureAddress os_get_procedure_address(OSModuleHandle module_handle, const char* procedure_name)
{
    OSProcedureAddress procedure_address = 0;

    ASSERT(os.get_procedure_address);
    procedure_address = os.get_procedure_address(module_handle, procedure_name);

    return procedure_address;
}

OSModuleHandle os_load_library(const char* module_name)
{
    OSModuleHandle module_handle = 0;

    ASSERT(os.load_library);
    module_handle = os.load_library(module_name);

    return module_handle;
}

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
