#ifndef H_OS_RANDOM_H

typedef struct OSRandomHandle
{
    u32 index;
} OSRandomHandle;

f32 os_random_unilateral(OSRandomHandle os_random_handle);
u32 os_random_next_u32(OSRandomHandle os_random_handle);
OSRandomHandle os_random_init(u32 value);

#define H_OS_RANDOM_H
#endif

