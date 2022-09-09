#include "types.h"
#include "utils.h"
#include "os_time.h"

#define INVALID_TIMER_HANDLE   0
#define MAX_TIMER_COUNT        11

typedef i64  OSTimeGetTick(void);
typedef i64  OSTimeGetFrequency(void);
typedef void OSTimeSleep(u32 milliseconds);

typedef struct OSTime
{
    OSTimeGetTick* get_tick;
    OSTimeGetFrequency* get_frequency;
    OSTimeSleep* sleep;
} OSTime;

#ifdef WIN32
#include "win32_time.h"

static OSTime os_time =
{
    .get_tick = &win32_time_get_tick,
    .get_frequency = &win32_time_get_frequency,
    .sleep = &win32_time_sleep,
};

#else
#error WIN32 must be defined.
#endif

typedef struct OSTimeTick
{
    i64 tick;
} OSTimeTick;

// NOTE: 0 is INVALID_TIMER
static OSTimeTick os_time_ticks[MAX_TIMER_COUNT];

static OSTimeTick* find_empty_time_tick(OSTimeTickHandle* os_time_tick_handle)
{
    OSTimeTickHandle found_time_tick_handle = INVALID_TIMER_HANDLE;
    OSTimeTick* found_time_tick = 0;

    for (u32 i = 1; i < MAX_TIMER_COUNT; ++i)
    {
        OSTimeTick* os_time_tick = os_time_ticks + i;
        
        if (os_time_tick->tick == 0)
        {
            found_time_tick = os_time_tick;
            found_time_tick_handle = i;
            break;
        }
    }

    ASSERT(found_time_tick_handle != INVALID_TIMER_HANDLE);
    ASSERT(found_time_tick && found_time_tick->tick == 0);

    *os_time_tick_handle = found_time_tick_handle;

    return found_time_tick;
}

f64 os_time_ticks_to_milliseconds(i64 tick)
{
    i64 frequency = os_time_get_frequency();
    f64 ticks = (f64)tick / ((f64)frequency / 1000.0);
    
    return ticks;
}

i64 os_time_get_tick(void)
{
    i64 tick = 0;

    ASSERT(os_time.get_tick);
    tick = os_time.get_tick();

    return tick;
}

i64 os_time_get_frequency(void)
{
    i64 frequency = 0;

    ASSERT(os_time.get_frequency);
    frequency = os_time.get_frequency();

    return frequency;
}

OSTimeTickHandle os_time_begin_tick(void)
{
    OSTimeTickHandle os_time_tick_handle = INVALID_TIMER_HANDLE;
    OSTimeTick* os_time_tick = find_empty_time_tick(&os_time_tick_handle);

    os_time_tick->tick = os_time_get_tick();
    
    return os_time_tick_handle;
}

f64 os_time_end_tick(OSTimeTickHandle os_time_tick_handle)
{
    OSTimeTick* os_time_tick = 0;
    i64 elapsed_ticks = 0;
    f64 elapsed_seconds = 0;
    
    ASSERT(os_time_tick_handle != INVALID_TIMER_HANDLE && os_time_tick_handle < MAX_TIMER_COUNT);
    
    os_time_tick = os_time_ticks + os_time_tick_handle;
    elapsed_ticks = os_time_get_tick() - os_time_tick->tick;
    elapsed_seconds = os_time_ticks_to_milliseconds(elapsed_ticks);
    os_time_tick->tick = 0;
    
    return elapsed_seconds;
}

void os_time_sleep(u32 milliseconds)
{
    ASSERT(os_time.sleep);
    os_time.sleep(milliseconds);
}
