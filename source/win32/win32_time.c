#include <Windows.h>
#include <timeapi.h>
#include "types.h"
#include "utils.h"
#include "win32_time.h"

#define TIMER_UNINITIALIZED 1

i64 win32_time_get_tick(void)
{
    static MMRESULT is_time_granular = TIMER_UNINITIALIZED;
    LARGE_INTEGER tick = { 0 };
    
    if (TIMER_UNINITIALIZED == is_time_granular)
        is_time_granular = timeBeginPeriod(1);
    ASSERT(TIMERR_NOERROR == is_time_granular);

    QueryPerformanceCounter(&tick);

    return (i64)tick.QuadPart;
}

i64 win32_time_get_frequency(void)
{
    // NOTE: Counts per second
    static LARGE_INTEGER frequency;

    if (!frequency.QuadPart)
        QueryPerformanceFrequency(&frequency);
    ASSERT(frequency.QuadPart);

    return (i64)frequency.QuadPart;
}

void win32_time_sleep(u32 milliseconds)
{
    Sleep(milliseconds);
}
