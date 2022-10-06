#ifndef H_OS_THREAD_H

#ifdef _WIN32
typedef unsigned long os_thread_callback_return;
typedef void* os_thread_callback_param;
#else
#error _WIN32 must be defined.
#endif

typedef uptr OSThreadHandle;
typedef os_thread_callback_return OSThreadProcedure(os_thread_callback_param parameter);

OSThreadHandle os_thread_create(OSThreadProcedure* thread_procedure, os_thread_callback_param parameter);
u32            os_thread_resume(OSThreadHandle thread_handle);
u32            os_thread_suspend(OSThreadHandle thread_handle);
b32            os_thread_wait_on_address(volatile void* address, void* compare_address,
                                         memory_size address_size, u32 milliseconds);
void           os_thread_wake_by_address(void* address);


#define H_OS_THREAD_H
#endif
