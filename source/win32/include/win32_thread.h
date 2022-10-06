#ifndef H_WIN32_THREAD_H

uptr win32_thread_create(unsigned long (*thread_procedure)(void*), void* parameter);
u32  win32_thread_resume(uptr thread_handle);
u32  win32_thread_suspend(uptr thread_handle);
b32  win32_thread_wait_on_address(volatile void* address, void* compare_address,
                                  memory_size address_size, u32 milliseconds);
void win32_thread_wake_by_address(void* address);

#define H_WIN32_THREAD_H
#endif
