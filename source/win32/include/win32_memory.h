#ifndef H_WIN32_MEMORY_H

void* win32_memory_reserve(memory_size size);
void* win32_memory_commit(void* memory, memory_size size);
b32   win32_memory_decommit(void* memory, memory_size size);
b32   win32_memory_release(void* memory);
void* win32_memory_heap_allocate(memory_size size, b32 zero_init);
void* win32_memory_heap_reallocate(void* memory, memory_size size, b32 zero_init);
b32   win32_memory_heap_release(void* memory);

#define H_WIN32_MEMORY_H
#endif
