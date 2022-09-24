#ifndef H_WIN32_THREAD_H

uptr win32_thread_create(u32 (*thread_procedure)(void*), void* parameter);
void win32_thread_exit(u32 exit_code);
u32  win32_thread_resume(uptr thread_handle);
u32  win32_thread_suspend(uptr thread_handle);

#define H_WIN32_THREAD_H
#endif
