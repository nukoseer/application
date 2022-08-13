#include "test.h"
#include "types.h"
#include "utils.h"
#include "mem.h"

static void stretchy_buffer_test(void)
{
    u32* arr = NULL;
    u32 i = 0;

    for (i = 0; i < 100; ++i)
    {
        BUFFER_PUSH(arr, i);
    }

    for (i = 0; i < BUFFER_LENGTH(arr); ++i)
    {
        ASSERT(i == arr[i]);
    }

    BUFFER_FREE(arr);
    ASSERT(NULL == arr);
}

static void doubly_linked_list_test(void)
{
    typedef struct Number_
    {
        struct Number_* prev;
        struct Number_* next;
        u32 i;
    } Number;
    typedef struct NumberList_
    {
        struct Number_* first;
        struct Number_* last;
    } NumberList;

    u32 i = 0;
    u32 count = 100;
    NumberList number_list = { 0 };
    MemoryArena* arena = NULL;
    TemporaryMemory temporary_memory = { 0 };
    
    arena = allocate_memory_arena(sizeof(Number) * count);

    {
        temporary_memory = begin_temporary_memory(arena);

        for (i = 0; i < count; ++i)
        {
            Number* number = PUSH_STRUCT(arena, Number);
            number->i = i;
        
            DLL_PUSH_BACK(number_list.first, number_list.last, number);
        }

        {
            Number* number = NULL;
            i = 0;
        
            for (number = number_list.first; number; number = number->next)
            {
                ASSERT(number->i == i++);
            }

            ASSERT(i == count);
        }
        
        end_temporary_memory(temporary_memory);
    }

    {
        memory_zero(&number_list, sizeof(NumberList));

        for (i = 0; i < count; ++i)
        {
            Number* number = PUSH_STRUCT(arena, Number);
            number->i = i;
        
            DLL_PUSH_FRONT(number_list.first, number_list.last, number);
        }

        {
            Number* number = NULL;
            i = 0;
        
            for (number = number_list.first; number; number = number->next)
            {
                ASSERT(number->i == (count - ++i));
            }

            ASSERT(i == count);
        }
    }

    release_memory_arena(arena);
}

void run_test(void)
{
    stretchy_buffer_test();
    doubly_linked_list_test();
}
