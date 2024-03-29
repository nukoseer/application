#include <Windows.h>
#include <timeapi.h>
#include "types.h"
#include "utils.h"
#include "os_time.h"
#include "win32_time.h"

#define TIMER_UNINITIALIZED 1

// TODO: Maybe we should use __rdtsc intrinsic directly instead of QueryPerformanceCounter()
u64 win32_time_get_tick(void)
{
    static MMRESULT is_time_granular = TIMER_UNINITIALIZED;
    LARGE_INTEGER tick = { 0 };

    if (TIMER_UNINITIALIZED == is_time_granular)
        is_time_granular = timeBeginPeriod(1);
    ASSERT(TIMERR_NOERROR == is_time_granular);

    QueryPerformanceCounter(&tick);

    return (u64)tick.QuadPart;
}

u64 win32_time_get_frequency(void)
{
    // NOTE: Counts per second
    static LARGE_INTEGER frequency;

    if (!frequency.QuadPart)
        QueryPerformanceFrequency(&frequency);
    ASSERT(frequency.QuadPart);

    return (u64)frequency.QuadPart;
}

void win32_time_sleep(u32 milliseconds)
{
    Sleep(milliseconds);
}

void win32_time_system_now(OSDateTime* os_system_time)
{
    SYSTEMTIME system_time = { 0 };

    GetSystemTime(&system_time);

    os_system_time->year         = system_time.wYear;
    os_system_time->month        = system_time.wMonth;
    os_system_time->day_of_week  = system_time.wDayOfWeek;
    os_system_time->day          = system_time.wDay;
    os_system_time->hour         = system_time.wHour;
    os_system_time->minute       = system_time.wMinute;
    os_system_time->second       = system_time.wSecond;
    os_system_time->milliseconds = system_time.wMilliseconds;
}

void win32_time_local_now(OSDateTime* os_local_time)
{
    SYSTEMTIME local_time = { 0 };

    GetLocalTime(&local_time);

    os_local_time->year         = local_time.wYear;
    os_local_time->month        = local_time.wMonth;
    os_local_time->day_of_week  = local_time.wDayOfWeek;
    os_local_time->day          = local_time.wDay;
    os_local_time->hour         = local_time.wHour;
    os_local_time->minute       = local_time.wMinute;
    os_local_time->second       = local_time.wSecond;
    os_local_time->milliseconds = local_time.wMilliseconds;
}

b32 win32_time_to_system(u64 time, OSDateTime* os_system_time)
{
    b32 result = FALSE;
    SYSTEMTIME system_time = { 0 };
    ULARGE_INTEGER ularge_time = { .QuadPart=time };
    FILETIME file_time =
    {
        .dwHighDateTime=ularge_time.HighPart,
        .dwLowDateTime=ularge_time.LowPart,
    };

    result = FileTimeToSystemTime(&file_time, &system_time);
    ASSERT(result);
    
    if (result)
    {
        os_system_time->year         = system_time.wYear;
        os_system_time->month        = system_time.wMonth;
        os_system_time->day_of_week  = system_time.wDayOfWeek;
        os_system_time->day          = system_time.wDay;
        os_system_time->hour         = system_time.wHour;
        os_system_time->minute       = system_time.wMinute;
        os_system_time->second       = system_time.wSecond;
        os_system_time->milliseconds = system_time.wMilliseconds;   
    }

    return result;
}

b32 win32_time_to_local(u64 time, OSDateTime* os_local_time)
{
    b32 result = FALSE;
    SYSTEMTIME local_time = { 0 };
    FILETIME local_file_time = { 0 };
    ULARGE_INTEGER ularge_time = { .QuadPart=time };
    FILETIME file_time =
    {
        .dwHighDateTime=ularge_time.HighPart,
        .dwLowDateTime=ularge_time.LowPart,
    };

    result = FileTimeToLocalFileTime(&file_time, &local_file_time);
    ASSERT(result);

    if (result)
    {
        result = FileTimeToSystemTime(&local_file_time, &local_time);
    }

    ASSERT(result);
    
    if (result)
    {
        os_local_time->year         = local_time.wYear;
        os_local_time->month        = local_time.wMonth;
        os_local_time->day_of_week  = local_time.wDayOfWeek;
        os_local_time->day          = local_time.wDay;
        os_local_time->hour         = local_time.wHour;
        os_local_time->minute       = local_time.wMinute;
        os_local_time->second       = local_time.wSecond;
        os_local_time->milliseconds = local_time.wMilliseconds;
    }

    return result;
}
