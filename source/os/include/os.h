#ifndef H_OS_H

#include "types.h"
#include "utils.h"
#include "memory_utils.h"
#include "maths.h"
#include "atomic.h"
#include "random.h"
#include "os_window.h"
#include "os_graphics.h"
#include "os_memory.h"
#include "os_time.h"
#include "os_io.h"
#include "os_log.h"
#include "os_thread.h"

typedef uptr OSModule;
typedef uptr OSProcedureAddress;

OSProcedureAddress os_get_procedure_address(OSModule module, const char* procedure_name);
OSModule os_load_library(const char* module_name);
b32 os_should_quit(void);
void os_destroy(void);
void os_init(void);

#define H_OS_H
#endif
