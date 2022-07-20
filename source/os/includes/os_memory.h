#ifndef _H_OS_MEMORY_H_

void* os_memory_allocate(u64 size);
void os_memory_release(void* memory);

#define _H_OS_MEMORY_H_
#endif
