#include <Windows.h>

#include "types.h"
#include "utils.h"
#include "win32_atomic.h"

i32 win32_atomic_add(volatile i32* addend, i32 value)
{
    i32 result = 0;

    result = InterlockedAdd((volatile LONG*)addend, value);

    return result;
}

i32 win32_atomic_increment(volatile i32* addend)
{
    i32 result = 0;
    
    result = InterlockedIncrement((volatile LONG*)addend);

    return result;
}

i32 win32_atomic_increment_acquire(volatile i32* addend)
{
    i32 result = 0;
    
    result = InterlockedIncrementAcquire((volatile LONG*)addend);

    return result;
}

i32 win32_atomic_increment_release(volatile i32* addend)
{
    i32 result = 0;
    
    result = InterlockedIncrementRelease((volatile LONG*)addend);

    return result;
}

i32 win32_atomic_decrement(volatile i32* addend)
{
    i32 result = 0;
    
    result = InterlockedDecrement((volatile LONG*)addend);

    return result;
}

i32 win32_atomic_decrement_acquire(volatile i32* addend)
{
    i32 result = 0;
    
    result = InterlockedDecrementAcquire((volatile LONG*)addend);

    return result;
}

i32 win32_atomic_decrement_release(volatile i32* addend)
{
    i32 result = 0;
    
    result = InterlockedDecrementRelease((volatile LONG*)addend);

    return result;
}

i32 win32_atomic_exchange(volatile i32* destination, i32 new_value)
{
    i32 initial_value = 0;
    
    initial_value = InterlockedExchange((volatile LONG*)destination, new_value);

    return initial_value;
}

i32 win32_atomic_exchange_acquire(volatile i32* destination, i32 new_value)
{
    i32 initial_value = 0;
    
    initial_value = InterlockedExchangeAcquire((volatile LONG*)destination, new_value);

    return initial_value;
}

i32 win32_atomic_compare_exchange(volatile i32* destination, i32 new_value, i32 comperand)
{
    i32 initial_value = 0;

    initial_value = InterlockedCompareExchange((volatile LONG*)destination, new_value, comperand);

    return initial_value;
}

i32 win32_atomic_compare_exchange_acquire(volatile i32* destination, i32 new_value, i32 comperand)
{
    i32 initial_value = 0;

    initial_value = InterlockedCompareExchangeAcquire((volatile LONG*)destination, new_value, comperand);

    return initial_value;
}

i32 win32_atomic_compare_exchange_release(volatile i32* destination, i32 new_value, i32 comperand)
{
    i32 initial_value = 0;

    initial_value = InterlockedCompareExchangeRelease((volatile LONG*)destination, new_value, comperand);

    return initial_value;
}

void* win32_atomic_compare_exchange_pointer(volatile void* destination, void* new_value, void* comperand)
{
    void* initial_value = 0;

    initial_value = InterlockedCompareExchangePointer(destination, new_value, comperand);

    return initial_value;
}

void* win32_atomic_compare_exchange_pointer_acquire(volatile void* destination, void* new_value, void* comperand)
{
    void* initial_value = 0;

    initial_value = InterlockedCompareExchangePointerAcquire(destination, new_value, comperand);

    return initial_value;
}

void* win32_atomic_compare_exchange_pointer_release(volatile void* destination, void* new_value, void* comperand)
{
    void* initial_value = 0;

    initial_value = InterlockedCompareExchangePointerRelease(destination, new_value, comperand);

    return initial_value;
}

