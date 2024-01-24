#include <stdio.h>
#include <string.h>
#include "test.h"
#include "types.h"
#include "utils.h"
#include "memory_utils.h"
#include "os.h"

static void stretchy_buffer_test(void)
{
    i32* arr = NULL;
    i32 i = 0;

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
        i64 i;
    } Number;
    typedef struct NumberList_
    {
        struct Number_* first;
        struct Number_* last;
    } NumberList;

    i32 i = 0;
    i32 count = 100;
    NumberList number_list = { 0 };
    MemoryArena* arena = NULL;
    TemporaryMemory temporary_memory = { 0 };
    
    arena = allocate_memory_arena(sizeof(Number) * (u32)count);

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

            FOREACH_BY_NAME(number_list.first, number)
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

            FOREACH_BY_NAME(number_list.first, number)
            {
                ASSERT(number->i == (count - ++i));
            }

            ASSERT(i == count);
        }
    }

    release_memory_arena(arena);
}

static void print_time(char* label, OSDateTime* os_date_time)
{
    char time_string[32] = { 0 };

    sprintf(time_string, "%02d/%02d/%04d %02d:%02d:%02d",
            os_date_time->day, os_date_time->month, os_date_time->year,
            os_date_time->hour, os_date_time->minute, os_date_time->second);
    fprintf(stderr, "%-24s", label);
    fprintf(stderr, "%s\n", time_string);
}

static void file_operation_test(void)
{
    OSIOFile os_io_file = 0;
    const char* file_name = "test_file.txt";
    b32 result = FALSE;
    OSDateTime os_date_time = { 0 };
    
    os_io_file = os_io_file_open(file_name, OS_IO_FILE_ACCESS_MODE_READ);
    ASSERT(os_io_file);

    result = os_io_file_get_creation_time(os_io_file, &os_date_time);
    ASSERT(result);

    print_time("File creation time: ", &os_date_time);
    
    result = os_io_file_close(os_io_file);
    ASSERT(result);

    {
        OSIOFileFind os_io_file_file_find = 0;
        u32 file_count = 0;
        i32 i = 0;
        
        os_io_file_file_find = os_io_file_find_begin("test_file.txt", &file_count);
        ASSERT(file_count == 1);

        for (i = 0; i < (i32)file_count; ++i)
        {
            os_io_file = os_io_file_find_and_open(os_io_file_file_find,
                                                         OS_IO_FILE_ACCESS_MODE_WRITE | OS_IO_FILE_ACCESS_MODE_READ);

            {
                const char write_buffer[] = "test";
                char read_buffer[sizeof(write_buffer)] = { 0 };
                u32 size = 0;

                size = os_io_file_write(os_io_file, write_buffer, sizeof(write_buffer));
                ASSERT(size == sizeof(write_buffer));

                result = os_io_file_get_last_access_time(os_io_file, &os_date_time);
                ASSERT(result);

                print_time("File last access time: ", &os_date_time);

                ASSERT(os_io_file_size(os_io_file) == os_io_file_pointer_get(os_io_file));
                
                size = os_io_file_pointer_reset(os_io_file);
                ASSERT(size == 0);

                size = os_io_file_read(os_io_file, read_buffer, sizeof(read_buffer));
                ASSERT(size == sizeof(read_buffer));

                ASSERT(!strcmp(write_buffer, read_buffer));
            }

            result = os_io_file_get_last_write_time(os_io_file, &os_date_time);
            ASSERT(result);

            print_time("File last write time: ", &os_date_time);
            
            result = os_io_file_close(os_io_file);
            ASSERT(result);
        }
        result = os_io_file_find_end(os_io_file_file_find);
        ASSERT(result);
    }

    result = os_io_file_delete(file_name);
    ASSERT(result);
}

static void time_test(void)
{
    OSDateTime os_local_time = os_time_local_now();

    print_time("Current time: ", &os_local_time);
}

static void defer_test(void)
{
    u32 x = 0;
    u32 y = 0;
        
    DEFER(x += 3)
    {
        ASSERT(x == 0);
    }
    ASSERT(x == 3);

    DEFER_LOOP(y += 5, y += 9)
    {
        ASSERT(y == 5);
    }
    ASSERT(y == 14);
}

void run_test(void)
{
    stretchy_buffer_test();
    doubly_linked_list_test();
    time_test();
    file_operation_test();
    defer_test();
}
