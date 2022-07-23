#ifndef H_OS_MEMORY_H

void* os_memory_allocate(u64 size);
void os_memory_release(void* memory);

#define H_OS_MEMORY_H
#endif
