#ifndef _H_WIN32_MEMORY_H_

void* win32_allocate_memory(u64 size);
void win32_release_memory(void* memory);

#define _H_WIN32_MEMORY_H_
#endif
