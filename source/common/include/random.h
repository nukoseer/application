#ifndef H_RANDOM_H

typedef struct RandomHandle
{
    u32 index;
} RandomHandle;

f32 random_unilateral(RandomHandle os_random_handle);
f32 random_bilateral(RandomHandle os_random_handle);
u32 random_next_u32(RandomHandle os_random_handle);
RandomHandle random_init(u32 value);

#define H_RANDOM_H
#endif

