#ifndef H_OS_MEMORY_H

void* os_memory_reserve(memory_size size);
void* os_memory_commit(void* memory, memory_size size);
b32   os_memory_decommit(void* memory, memory_size size);
void* os_memory_allocate(memory_size size);
b32   os_memory_release(void* memory);
void* os_memory_heap_allocate(memory_size size);
void* os_memory_heap_reallocate(void* memory, memory_size size);
b32   os_memory_heap_release(void* memory);

#define H_OS_MEMORY_H
#endif
