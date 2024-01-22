#ifndef H_MEMORY_H

#define DEFAULT_ALIGNMENT 4

#define PUSH_SIZE(memory_arena, size) (push_size(memory_arena, size, DEFAULT_ALIGNMENT))
#define PUSH_SIZE_N(memory_arena, size, alignment) (push_size(memory_arena, size, alignment))
#define PUSH_ARRAY(memory_arena, count, type) (push_size(memory_arena, ((count) * sizeof(type)), DEFAULT_ALIGNMENT))
#define PUSH_ARRAY_N(memory_arena, count, type, alignment) (push_size(memory_arena, ((count) * sizeof(type)), alignment))
#define PUSH_STRUCT(memory_arena, type) ((type*)(push_size(memory_arena, sizeof(type), DEFAULT_ALIGNMENT)))
#define PUSH_STRUCT_N(memory_arena, type, alignment) ((type*)(push_size(memory_arena, sizeof(type), alignment)))
#define PUSH_ARRAY_ZERO(memory_arena, count, type) (push_size_zero(memory_arena, ((count) * sizeof(type)), DEFAULT_ALIGNMENT))
#define PUSH_ARRAY_ZERO_N(memory_arena, count, type, alignment) (push_size_zero(memory_arena, ((count) * sizeof(type)), alignment))
#define PUSH_STRUCT_ZERO(memory_arena, type) ((type*)(push_size_zero(memory_arena, sizeof(type), DEFAULT_ALIGNMENT)))
#define PUSH_STRUCT_ZERO_N(memory_arena, type, alignment) ((type*)(push_size_zero(memory_arena, sizeof(type), alignment)))
#define STRUCT_ZERO(memory, type) (memory_zero(memory, sizeof(type)))

typedef struct MemoryArena
{
    void* memory;
    memory_size max_size;
    memory_size used_size;
} MemoryArena;

typedef struct TemporaryMemory
{
    MemoryArena* arena;
    memory_size initial_size;
} TemporaryMemory;

MemoryArena* allocate_memory_arena(memory_size max_size);
void release_memory_arena(MemoryArena* memory_arena);
TemporaryMemory begin_temporary_memory(MemoryArena* memory_arena);
void end_temporary_memory(TemporaryMemory temporary_memory);
void* push_size(MemoryArena* arena, memory_size size, memory_size alignment);
void* push_size_zero(MemoryArena* arena, memory_size size, memory_size alignment);
void memory_zero(void* memory, memory_size size);

#define H_MEMORY_H
#endif
