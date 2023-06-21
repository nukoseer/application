#ifndef H_ATOMICS_H

#ifdef _MSC_VER

// NOTE: https://learn.microsoft.com/en-us/cpp/intrinsics/x64-amd64-intrinsics-list?view=msvc-170
#include <intrin.h>

// TODO: We got rid of acquire/release semantics for sake of simplicity.
// If we really need them we can reimplement them in the future.

#ifdef _M_AMD64
#define ATOMIC_EXCHANGE_ADD(addend, value) _InterlockedExchangeAdd(addend, value)
#define ATOMIC_INCREMENT(addend) _InterlockedIncrement(addend)
#define ATOMIC_DECREMENT(addend) _InterlockedDecrement(addend)
#define ATOMIC_AND(value, mask) _InterlockedAnd(value, mask)
#define ATOMIC_EXCHANGE(destination, new_value) _InterlockedExchange(destination, new_value)
#define ATOMIC_COMPARE_EXCHANGE(destination, new_value, comperand) _InterlockedCompareExchange(destination, new_value, comperand)
#define ATOMIC_COMPARE_EXCHANGE_POINTER(destination, new_value, comperand) _InterlockedCompareExchangePointer(destination, new_value, comperand)
#endif

// TODO: Support different platforms and compilers if needed.
#ifdef _M_ARM64
#define ATOMIC_EXCHANGE_ADD(addend, value)
#define ATOMIC_INCREMENT(addend)
#define ATOMIC_DECREMENT(addend)
#define ATOMIC_AND(value, mask)
#define ATOMIC_EXCHANGE(destination, new_value)
#define ATOMIC_COMPARE_EXCHANGE(destination, new_value, comperand)
#define ATOMIC_COMPARE_EXCHANGE_POINTER(destination, new_value, comperand)
#endif

static inline i32 atomic_exchange_add(volatile i32* addend, i32 value)
{
    i32 initial_value = 0;

    initial_value = ATOMIC_EXCHANGE_ADD((volatile long*)addend, value);

    return initial_value;
}

static inline i32 atomic_increment(volatile i32* addend)
{
    i32 result = 0;

    result = ATOMIC_INCREMENT((volatile long*)addend);

    return result;
}

static inline i32 atomic_decrement(volatile i32* addend)
{
    i32 result = 0;

    result = ATOMIC_DECREMENT((volatile long*)addend);

    return result;
}

static inline i32 atomic_and(volatile i32* value, i32 mask)
{
    i32 initial_value = 0;

    initial_value = ATOMIC_AND((volatile long*)value, mask);

    return initial_value;
}

static inline i32 atomic_exchange(volatile i32* destination, i32 new_value)
{
    i32 initial_value = 0;

    initial_value = ATOMIC_EXCHANGE((volatile long*)destination, new_value);

    return initial_value;
}

static inline i32 atomic_compare_exchange(volatile i32* destination, i32 new_value, i32 comperand)
{
    i32 initial_value = 0;

    initial_value = ATOMIC_COMPARE_EXCHANGE((volatile long*)destination, new_value, comperand);

    return initial_value;
}

static inline void* atomic_compare_exchange_pointer(volatile void* destination, void* new_value, void* comperand)
{
    void* initial_value = 0;

    initial_value = ATOMIC_COMPARE_EXCHANGE_POINTER(destination, new_value, comperand);

    return initial_value;
}

#endif

#define H_ATOMICS_H
#endif
