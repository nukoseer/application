#ifndef H_WIN32_MEMORY_H

void* win32_memory_allocate(u64 size);
void win32_memory_release(void* memory);

#define H_WIN32_MEMORY_H
#endif
