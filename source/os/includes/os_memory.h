#ifndef _H_OS_MEMORY_H_

void* os_allocate_memory(u64 size);
void os_release_memory(void* memory);

#define _H_OS_MEMORY_H_
#endif
