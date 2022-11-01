#include <stdio.h>
#include "test.h"
#include "types.h"
#include "utils.h"
#include "mem.h"
#include "os.h"

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
        u64 i;
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

static void file_operation_test(void)
{
    OSIOFileHandle os_io_file_handle = 0;
    const char* file_name = "test_file.txt";
    b32 result = FALSE;
    
    os_io_file_handle = os_io_file_open(file_name, OS_IO_FILE_ACCESS_MODE_READ);
    ASSERT(os_io_file_handle);
    result = os_io_file_close(os_io_file_handle);
    ASSERT(result);

    {
        OSIOFileFindHandle os_io_file_find_handle = 0;
        u32 file_count = 0;
        u32 i = 0;
        
        os_io_file_find_handle = os_io_file_find_begin("test_file.txt", &file_count);
        ASSERT(file_count == 1);

        for (i = 0; i < file_count; ++i)
        {
            os_io_file_handle = os_io_file_find_and_open(os_io_file_find_handle,
                                                         OS_IO_FILE_ACCESS_MODE_WRITE | OS_IO_FILE_ACCESS_MODE_READ);

            {
                const char buffer[] = "test";
                u32 bytes_written = 0;

                bytes_written = os_io_file_write(os_io_file_handle, buffer, sizeof(buffer));
                ASSERT(bytes_written == sizeof(buffer));
            }
            
            result = os_io_file_close(os_io_file_handle);
            ASSERT(result);
        }
        result = os_io_file_find_end(os_io_file_find_handle);
        ASSERT(result);
    }

    result = os_io_file_delete(file_name);
    ASSERT(result);
}

static void time_test(void)
{
    char time_string[32] = { 0 };
    OSDateTime os_local_time = os_time_local_now();

    sprintf(time_string, "%02d/%02d/%04d %02d:%02d:%02d",
            os_local_time.day, os_local_time.month, os_local_time.year,
            os_local_time.hour, os_local_time.minute, os_local_time.second);
    fprintf(stderr, "%s\n", time_string);
}

void run_test(void)
{
    stretchy_buffer_test();
    doubly_linked_list_test();
    file_operation_test();
    time_test();
}
