#ifndef H_OS_ATOMIC_H

i32   os_atomic_add(volatile i32* addend, i32 value);
i32   os_atomic_increment(volatile i32* addend);
i32   os_atomic_increment_acquire(volatile i32* addend);
i32   os_atomic_increment_release(volatile i32* addend);
i32   os_atomic_decrement(volatile i32* addend);
i32   os_atomic_decrement_acquire(volatile i32* addend);
i32   os_atomic_decrement_release(volatile i32* addend);
i32   os_atomic_exchange(volatile i32* destination, i32 new_value);
i32   os_atomic_exchange_acquire(volatile i32* destination, i32 new_value);
i32   os_atomic_compare_exchange(volatile i32* destination, i32 new_value, i32 comperand);
i32   os_atomic_compare_exchange_acquire(volatile i32* destination, i32 new_value, i32 comperand);
i32   os_atomic_compare_exchange_release(volatile i32* destination, i32 new_value, i32 comperand);
void* os_atomic_compare_exchange_pointer(volatile void* destination, void* new_value, void* comperand);
void* os_atomic_compare_exchange_pointer_acquire(volatile void* destination, void* new_value, void* comperand);
void* os_atomic_compare_exchange_pointer_release(volatile void* destination, void* new_value, void* comperand);

#define H_OS_ATOMIC_H
#endif