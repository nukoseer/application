#ifndef H_WIN32_H

uptr win32_get_procedure_address(uptr module, const char* procedure_name);
uptr win32_load_library(const char* module_name);
b32 win32_should_quit(void);
void win32_destroy(void);
void win32_init(void);

#define H_WIN32_H
#endif
