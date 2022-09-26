#include "types.h"
#include "utils.h"
#include "os_atomic.h"

typedef i32   OSAtomicAdd(volatile i32* addend, i32 value);
typedef i32   OSAtomicIncrement(volatile i32* addend);
typedef i32   OSAtomicIncrementAcquire(volatile i32* addend);
typedef i32   OSAtomicIncrementRelease(volatile i32* addend);
typedef i32   OSAtomicDecrement(volatile i32* addend);
typedef i32   OSAtomicDecrementAcquire(volatile i32* addend);
typedef i32   OSAtomicDecrementRelease(volatile i32* addend);
typedef i32   OSAtomicExchange(volatile i32* destination, i32 new_value);
typedef i32   OSAtomicExchangeAcquire(volatile i32* destination, i32 new_value);
typedef i32   OSAtomicCompareExchange(volatile i32* destination, i32 new_value, i32 comperand);
typedef i32   OSAtomicCompareExchangeAcquire(volatile i32* destination, i32 new_value, i32 comperand);
typedef i32   OSAtomicCompareExchangeRelease(volatile i32* destination, i32 new_value, i32 comperand);
typedef void* OSAtomicCompareExchangePointer(volatile void* destination, void* new_value, void* comperand);
typedef void* OSAtomicCompareExchangePointerAcquire(volatile void* destination, void* new_value, void* comperand);
typedef void* OSAtomicCompareExchangePointerRelease(volatile void* destination, void* new_value, void* comperand);

typedef struct OSAtomic
{
    OSAtomicAdd* add;
    OSAtomicIncrement* increment;
    OSAtomicIncrementAcquire* increment_acquire;
    OSAtomicIncrementRelease* increment_release;
    OSAtomicDecrement* decrement;
    OSAtomicDecrementAcquire* decrement_acquire;
    OSAtomicDecrementRelease* decrement_release;
    OSAtomicExchange* exchange;
    OSAtomicExchangeAcquire* exchange_acquire;
    OSAtomicCompareExchange* compare_exchange;
    OSAtomicCompareExchangeAcquire* compare_exchange_acquire;
    OSAtomicCompareExchangeRelease* compare_exchange_release;
    OSAtomicCompareExchangePointer* compare_exchange_pointer;
    OSAtomicCompareExchangePointerAcquire* compare_exchange_pointer_acquire;
    OSAtomicCompareExchangePointerRelease* compare_exchange_pointer_release;
} OSAtomic;

#ifdef _WIN32

#include "win32_atomic.h"

static OSAtomic os_atomic =
{
    .add = &win32_atomic_add,
    .increment = &win32_atomic_increment,
    .increment_acquire = &win32_atomic_increment_acquire,
    .increment_release = &win32_atomic_increment_release,
    .decrement = &win32_atomic_decrement,
    .decrement_acquire = &win32_atomic_decrement_acquire,
    .decrement_release = &win32_atomic_decrement_release,
    .exchange = &win32_atomic_exchange,
    .exchange_acquire = &win32_atomic_exchange_acquire,
    .compare_exchange = &win32_atomic_compare_exchange,
    .compare_exchange_acquire = &win32_atomic_compare_exchange_acquire,
    .compare_exchange_release = &win32_atomic_compare_exchange_release,
    .compare_exchange_pointer = &win32_atomic_compare_exchange_pointer,
    .compare_exchange_pointer_acquire = &win32_atomic_compare_exchange_pointer_acquire,
    .compare_exchange_pointer_release = &win32_atomic_compare_exchange_pointer_release,
};

#else
#error _WIN32 must be defined.
#endif

i32 os_atomic_add(volatile i32* addend, i32 value)
{
    i32 result = 0;

    ASSERT(os_atomic.add);
    result = os_atomic.add(addend, value);

    return result;
}

i32 os_atomic_increment(volatile i32* addend)
{
    i32 result = 0;

    ASSERT(os_atomic.increment);
    result = os_atomic.increment(addend);

    return result;
}

i32 os_atomic_increment_acquire(volatile i32* addend)
{
    i32 result = 0;

    ASSERT(os_atomic.increment_acquire);
    result = os_atomic.increment_acquire(addend);

    return result;
}

i32 os_atomic_increment_release(volatile i32* addend)
{
    i32 result = 0;

    ASSERT(os_atomic.increment_release);
    result = os_atomic.increment_release(addend);

    return result;
}

i32 os_atomic_decrement(volatile i32* addend)
{
    i32 result = 0;

    ASSERT(os_atomic.decrement);
    result = os_atomic.decrement(addend);

    return result;
}

i32 os_atomic_decrement_acquire(volatile i32* addend)
{
    i32 result = 0;

    ASSERT(os_atomic.decrement_acquire);
    result = os_atomic.decrement_acquire(addend);

    return result;
}

i32 os_atomic_decrement_release(volatile i32* addend)
{
    i32 result = 0;

    ASSERT(os_atomic.decrement_release);
    result = os_atomic.decrement_release(addend);

    return result;
}

i32 os_atomic_exchange(volatile i32* destination, i32 new_value)
{
    i32 initial_value = 0;

    ASSERT(os_atomic.exchange);
    initial_value = os_atomic.exchange(destination, new_value);

    return initial_value;
}

i32 os_atomic_exchange_acquire(volatile i32* destination, i32 new_value)
{
    i32 initial_value = 0;

    ASSERT(os_atomic.exchange_acquire);
    initial_value = os_atomic.exchange_acquire(destination, new_value);

    return initial_value;
}

i32 os_atomic_compare_exchange(volatile i32* destination, i32 new_value, i32 comperand)
{
    i32 initial_value = 0;

    ASSERT(os_atomic.compare_exchange);
    initial_value = os_atomic.compare_exchange(destination, new_value, comperand);

    return initial_value;
}

i32 os_atomic_compare_exchange_acquire(volatile i32* destination, i32 new_value, i32 comperand)
{
    i32 initial_value = 0;

    ASSERT(os_atomic.compare_exchange_acquire);
    initial_value = os_atomic.compare_exchange_acquire(destination, new_value, comperand);

    return initial_value;
}

i32 os_atomic_compare_exchange_release(volatile i32* destination, i32 new_value, i32 comperand)
{
    i32 initial_value = 0;

    ASSERT(os_atomic.compare_exchange_release);
    initial_value = os_atomic.compare_exchange_release(destination, new_value, comperand);

    return initial_value;
}

void* os_atomic_compare_exchange_pointer(volatile void* destination, void* new_value, void* comperand)
{
    void* initial_value = 0;

    ASSERT(os_atomic.compare_exchange_pointer);
    initial_value = os_atomic.compare_exchange_pointer(destination, new_value, comperand);

    return initial_value;
}

void* os_atomic_compare_exchange_pointer_acquire(volatile void* destination, void* new_value, void* comperand)
{
    void* initial_value = 0;

    ASSERT(os_atomic.compare_exchange_pointer_acquire);
    initial_value = os_atomic.compare_exchange_pointer_acquire(destination, new_value, comperand);

    return initial_value;
}

void* os_atomic_compare_exchange_pointer_release(volatile void* destination, void* new_value, void* comperand)
{
    void* initial_value = 0;

    ASSERT(os_atomic.compare_exchange_pointer_release);
    initial_value = os_atomic.compare_exchange_pointer_release(destination, new_value, comperand);

    return initial_value;
}
