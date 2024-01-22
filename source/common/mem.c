#include "types.h"
#include "utils.h"
#include "os_memory.h"
#include "mem.h"

MemoryArena* allocate_memory_arena(memory_size max_size)
{
    MemoryArena* arena = (MemoryArena*)os_memory_allocate(max_size + sizeof(MemoryArena));

    arena->memory = (u8*)arena + sizeof(MemoryArena);
    arena->used_size = 0;
    arena->max_size = max_size;

    return arena;
}

void release_memory_arena(MemoryArena* arena)
{
    os_memory_release(arena);
}

TemporaryMemory begin_temporary_memory(MemoryArena* arena)
{
    TemporaryMemory temporary_memory = { 0 };

    temporary_memory.arena = arena;
    temporary_memory.initial_size = arena->used_size;

    return temporary_memory;
}

void end_temporary_memory(TemporaryMemory temporary_memory)
{
    temporary_memory.arena->used_size = temporary_memory.initial_size;
}

// TODO: Probably, we should also handle alignment in these functions.
void* push_size(MemoryArena* arena, memory_size size)
{
    void* result = 0;
    
    ASSERT(arena->used_size + size <= arena->max_size);
    result = (u8*)arena->memory + arena->used_size;
    
    arena->used_size += size;

    return result;
}

void* push_size_zero(MemoryArena* arena, memory_size size)
{
    void* result = push_size(arena, size);
    
    memory_zero(result, size);

    return result;
}

// NOTE: We can use memset.
void memory_zero(void* memory, memory_size size)
{
    u8* pointer = (u8*)memory;
    
    while (size--)
    {
        *pointer++ = 0;
    }
}
