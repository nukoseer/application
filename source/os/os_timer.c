#include "types.h"
#include "utils.h"
#include "os_timer.h"

#define INVALID_TIMER_HANDLE   0
#define MAX_TIMER_COUNT        11

typedef i64 OSTimerGetTick(void);
typedef i64 OSTimerGetFrequency(void);

typedef struct OSTimer
{
    OSTimerGetTick* get_tick;
    OSTimerGetFrequency* get_frequency;
} OSTimer;

#ifdef WIN32
#include "win32_timer.h"

static OSTimer os_timer =
{
    .get_tick = &win32_timer_get_tick,
    .get_frequency = &win32_timer_get_frequency,
};

#else
#error WIN32 must be defined.
#endif

typedef struct OSTimerTick
{
    i64 tick;
} OSTimerTick;

// NOTE: 0 is INVALID_TIMER
static OSTimerTick os_timer_ticks[MAX_TIMER_COUNT];

static OSTimerTick* find_empty_timer_tick(OSTimerHandle* os_timer_handle)
{
    OSTimerHandle found_timer_handle = INVALID_TIMER_HANDLE;
    OSTimerTick* found_timer_tick = 0;

    for (u32 i = 1; i < MAX_TIMER_COUNT; ++i)
    {
        OSTimerTick* os_timer_tick = os_timer_ticks + i;
        
        if (os_timer_tick->tick == 0)
        {
            found_timer_tick = os_timer_tick;
            found_timer_handle = i;
            break;
        }
    }

    ASSERT(found_timer_handle != INVALID_TIMER_HANDLE);
    ASSERT(found_timer_tick && found_timer_tick->tick == 0);

    *os_timer_handle = found_timer_handle;

    return found_timer_tick;
}

f64 os_timer_ticks_to_milliseconds(i64 tick)
{
    i64 frequency = os_timer_get_frequency();
    f64 ticks = (f64)tick / ((f64)frequency / 1000.0);
    
    return ticks;
}

i64 os_timer_get_tick(void)
{
    i64 tick = 0;

    ASSERT(os_timer.get_tick);
    tick = os_timer.get_tick();

    return tick;
}

i64 os_timer_get_frequency(void)
{
    i64 frequency = 0;

    ASSERT(os_timer.get_frequency);
    frequency = os_timer.get_frequency();

    return frequency;
}

OSTimerHandle os_timer_begin(void)
{
    OSTimerHandle os_timer_handle = INVALID_TIMER_HANDLE;
    OSTimerTick* os_timer_tick = find_empty_timer_tick(&os_timer_handle);

    os_timer_tick->tick = os_timer_get_tick();
    
    return os_timer_handle;
}

f64 os_timer_end(OSTimerHandle os_timer_handle)
{
    OSTimerTick* os_timer_tick = 0;
    i64 elapsed_ticks = 0;
    f64 elapsed_seconds = 0;
    
    ASSERT(os_timer_handle != INVALID_TIMER_HANDLE && os_timer_handle < MAX_TIMER_COUNT);
    
    os_timer_tick = os_timer_ticks + os_timer_handle;
    elapsed_ticks = os_timer_get_tick() - os_timer_tick->tick;
    elapsed_seconds = os_timer_ticks_to_milliseconds(elapsed_ticks);
    os_timer_tick->tick = 0;
    
    return elapsed_seconds;
}
