#include "types.h"
#include "utils.h"
#include "memory_utils.h"
#include "os.h"

typedef void OSInit(void);
typedef b32 OSQuit(void);
typedef void OSDestroy(void);
typedef OSModule OSLoadLibrary(const char* module_name);
typedef OSProcedureAddress OSGetProcedureAddress(OSModule module, const char* procedure_name);

#ifdef _WIN32
#include "win32.h"

typedef struct OSTable
{
    OSGetProcedureAddress* get_procedure_address;
    OSLoadLibrary* load_library;
    OSQuit* should_quit;
    OSDestroy* destroy;
    OSInit* init;
} OSTable;

static OSTable os_table =
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

OSProcedureAddress os_get_procedure_address(OSModule module, const char* procedure_name)
{
    OSProcedureAddress procedure_address = 0;

    ASSERT(os_table.get_procedure_address);
    procedure_address = os_table.get_procedure_address(module, procedure_name);

    return procedure_address;
}

OSModule os_load_library(const char* module_name)
{
    OSModule module = 0;

    ASSERT(os_table.load_library);
    module = os_table.load_library(module_name);

    return module;
}

b32 os_should_quit(void)
{
    b32 quit = FALSE;

    ASSERT(os_table.should_quit);
    quit = os_table.should_quit();

    return quit;
}

void os_destroy(void)
{
    ASSERT(os_table.destroy);
    os_table.destroy();
}

// TODO: Consider getting rid of this. We should not have to call os_init explicitly.
// Handle this internally, maybe when window is created??
void os_init(void)
{
    ASSERT(os_table.init);
    os_table.init();
    os_window_init();
}
