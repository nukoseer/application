#ifndef H_RANDOM_H

typedef struct Random
{
    u32 index;
} Random;

f32 random_unilateral(Random os_random);
f32 random_bilateral(Random os_random);
u32 random_next_u32(Random os_random);
Random random_init(u32 value);

#define H_RANDOM_H
#endif

