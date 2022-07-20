#ifndef _H_WIN32_MEMORY_H_

void* win32_memory_allocate(u64 size);
void win32_memory_release(void* memory);

#define _H_WIN32_MEMORY_H_
#endif
