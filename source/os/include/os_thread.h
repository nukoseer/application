#ifndef H_OS_THREAD_H

typedef uptr OSThreadHandle;
typedef u32 OSThreadProcedure(void* parameter);

OSThreadHandle os_thread_create(OSThreadProcedure* thread_procedure, void* parameter);
void os_thread_exit(u32 exit_code);
u32  os_thread_resume(OSThreadHandle thread_handle);
u32  os_thread_suspend(OSThreadHandle thread_handle);

#define H_OS_THREAD_H
#endif
