#include "types.h"
#include "utils.h"
#include "os_memory.h"
#include "memory_utils.h"

// NOTE: Alignment must be power of 2.
memory_size get_alignment_offset(MemoryArena* arena, memory_size alignment)
{
    memory_size alignment_offset = 0;
    memory_size memory = (memory_size)arena->memory + arena->used_size;
    memory_size alignment_mask = alignment - 1;

    if (memory & alignment_mask)
    {
        alignment_offset = alignment - (memory & alignment_mask);
    }

    return alignment_offset;
}

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

void* push_size(MemoryArena* arena, memory_size size, memory_size alignment)
{
    void* result = 0;
    memory_size alignment_offset = get_alignment_offset(arena, alignment);
    
    ASSERT(arena->used_size + size + alignment_offset <= arena->max_size);
    result = (u8*)arena->memory + arena->used_size + alignment_offset;
    
    arena->used_size += size + alignment_offset;

    return result;
}

void* push_size_zero(MemoryArena* arena, memory_size size, memory_size alignment)
{
    void* result = push_size(arena, size, alignment);
    
    memory_zero(result, size);

    return result;
}

