#include "types.h"
#include "utils.h"
#include "os_time.h"

#define INVALID_TIMER_HANDLE   0
#define MAX_TIMER_COUNT        11

typedef u64  OSTimeGetTick(void);
typedef u64  OSTimeGetFrequency(void);
typedef void OSTimeSleep(u32 milliseconds);
typedef void OSTimeSystemNow(OSDateTime* os_system_time);
typedef void OSTimeLocalNow(OSDateTime* os_local_time);

typedef struct OSTimeTable
{
    OSTimeGetTick* get_tick;
    OSTimeGetFrequency* get_frequency;
    OSTimeSleep* sleep;
    OSTimeSystemNow* system_now;
    OSTimeLocalNow* local_now;
} OSTimeTable;

#ifdef _WIN32
#include "win32_time.h"

static OSTimeTable os_time_table =
{
    .get_tick = &win32_time_get_tick,
    .get_frequency = &win32_time_get_frequency,
    .sleep = &win32_time_sleep,
    .system_now = &win32_time_system_now,
    .local_now = &win32_time_local_now,
};

#else
#error _WIN32 must be defined.
#endif

typedef struct OSTimeTick
{
    u64 tick;
} OSTimeTick;

// NOTE: 0 is INVALID_TIMER
static OSTimeTick os_time_ticks[MAX_TIMER_COUNT];

static OSTimeTick* find_empty_time_tick(OSTimeTickHandle* os_time_tick_handle)
{
    OSTimeTickHandle found_time_tick_handle = INVALID_TIMER_HANDLE;
    OSTimeTick* found_time_tick = 0;

    for (i32 i = 1; i < MAX_TIMER_COUNT; ++i)
    {
        OSTimeTick* os_time_tick = os_time_ticks + i;

        if (os_time_tick->tick == 0)
        {
            found_time_tick = os_time_tick;
            found_time_tick_handle = (u32)i;
            break;
        }
    }

    ASSERT(found_time_tick_handle != INVALID_TIMER_HANDLE);
    ASSERT(found_time_tick && found_time_tick->tick == 0);

    *os_time_tick_handle = found_time_tick_handle;

    return found_time_tick;
}

f64 os_time_tick_to_milliseconds(u64 tick)
{
    u64 frequency = os_time_get_frequency();
    f64 milliseconds = 1000.0 * tick / (f64)frequency;

    return milliseconds;
}

f64 os_time_tick_to_seconds(u64 tick)
{
    u64 frequency = os_time_get_frequency();
    f64 seconds = tick / (f64)frequency;

    return seconds;
}

f64 os_time_get_elapsed_seconds(u64 begin_tick, u64 end_tick)
{
    u64 frequency = os_time_get_frequency();
    f64 seconds_elapsed = ((f64)end_tick - (f64)begin_tick) / (f64)frequency;

    return seconds_elapsed;
}

u64 os_time_get_tick(void)
{
    u64 tick = 0;

    ASSERT(os_time_table.get_tick);
    tick = os_time_table.get_tick();

    return tick;
}

u64 os_time_get_frequency(void)
{
    u64 frequency = 0;

    ASSERT(os_time_table.get_frequency);
    frequency = os_time_table.get_frequency();

    return frequency;
}

// TODO: Do we really need this begin_tick, end_tick thing? 
OSTimeTickHandle os_time_begin_tick(void)
{
    OSTimeTickHandle os_time_tick_handle = INVALID_TIMER_HANDLE;
    OSTimeTick* os_time_tick = find_empty_time_tick(&os_time_tick_handle);

    os_time_tick->tick = os_time_get_tick();

    return os_time_tick_handle;
}

u64 os_time_end_tick(OSTimeTickHandle os_time_tick_handle)
{
    OSTimeTick* os_time_tick = 0;
    u64 elapsed_ticks = 0;

    ASSERT(os_time_tick_handle != INVALID_TIMER_HANDLE && os_time_tick_handle < MAX_TIMER_COUNT);

    os_time_tick = os_time_ticks + os_time_tick_handle;
    elapsed_ticks = os_time_get_tick() - os_time_tick->tick;
    os_time_tick->tick = 0;

    return elapsed_ticks;
}

void os_time_sleep(u32 milliseconds)
{
    ASSERT(os_time_table.sleep);
    os_time_table.sleep(milliseconds);
}

OSDateTime os_time_system_now(void)
{
    OSDateTime os_system_time = { 0 };

    ASSERT(os_time_table.system_now);
    os_time_table.system_now(&os_system_time);

    return os_system_time;
}

OSDateTime os_time_local_now(void)
{
    OSDateTime os_local_time = { 0 };

    ASSERT(os_time_table.local_now);
    os_time_table.local_now(&os_local_time);

    return os_local_time;
}
