#include "types.h"
#include "utils.h"
#include "os_memory.h"
#include "memory.h"

MemoryArena* allocate_memory_arena(u64 max_size)
{
    MemoryArena* memory_arena = (MemoryArena*)os_allocate_memory(max_size);

    memory_arena->memory = (u8*)memory_arena + sizeof(MemoryArena);
    memory_arena->used_size = 0;
    memory_arena->max_size = max_size;

    return memory_arena;
}

void release_memory_arena(MemoryArena* memory_arena)
{
    os_release_memory(memory_arena);
}

TemporaryMemory begin_temporary_memory(MemoryArena* memory_arena)
{
    TemporaryMemory temporary_memory = { 0 };

    temporary_memory.memory_arena = memory_arena;
    temporary_memory.initial_size = memory_arena->used_size;

    return temporary_memory;
}

void end_temporary_memory(TemporaryMemory temporary_memory)
{
    temporary_memory.memory_arena->used_size = temporary_memory.initial_size;
}

void* push_size(MemoryArena* memory_arena, u64 size)
{
    ASSERT(memory_arena->used_size + size <= memory_arena->max_size);
    void* result = (u8*)memory_arena->memory + memory_arena->used_size;
    
    memory_arena->used_size += size;

    return result;
}

void* push_size_zero(MemoryArena* memory_arena, u64 size)
{
    ASSERT(memory_arena->used_size + size <= memory_arena->max_size);
    void* result = (u8*)memory_arena->memory + memory_arena->used_size;
    
    memory_arena->used_size += size;
    memory_zero(result, size);

    return result;
}

// NOTE: We can use memset.
void memory_zero(void* memory, u64 size)
{
    u8* pointer = (u8*)memory;
    
    while (size--)
    {
        *pointer++ = 0;
    }
}
