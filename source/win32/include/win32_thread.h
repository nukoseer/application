#ifndef H_WIN32_THREAD_H

uptr win32_thread_create(unsigned long (*thread_procedure)(void*), void* parameter);
u32  win32_thread_resume(uptr thread_handle);
u32  win32_thread_suspend(uptr thread_handle);

#define H_WIN32_THREAD_H
#endif
