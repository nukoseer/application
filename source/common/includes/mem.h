#ifndef H_MEMORY_H

#define push_array(memory_arena, count, type) (push_size(memory_arena, ((count) * sizeof(type))))
#define push_struct(memory_arena, type) ((type*)(push_size(memory_arena, sizeof(type))))
#define push_array_zero(memory_arena, count, type) (push_size_zero(memory_arena, ((count) * sizeof(type))))
#define push_struct_zero(memory_arena, type) ((type*)(push_size_zero(memory_arena, sizeof(type))))

typedef struct MemoryArena
{
    void* memory;
    memory_size max_size;
    memory_size used_size;
} MemoryArena;

typedef struct TemporaryMemory
{
    MemoryArena* memory_arena;
    memory_size initial_size;
} TemporaryMemory;

MemoryArena* allocate_memory_arena(memory_size max_size);
void release_memory_arena(MemoryArena* memory_arena);
TemporaryMemory begin_temporary_memory(MemoryArena* memory_arena);
void end_temporary_memory(TemporaryMemory temporary_memory);
void* push_size(MemoryArena* memory_arena, memory_size size);
void* push_size_zero(MemoryArena* memory_arena, memory_size size);
void memory_zero(void* memory, memory_size size);

#define H_MEMORY_H
#endif
