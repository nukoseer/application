#ifndef H_OS_THREAD_H

typedef uptr OSThreadHandle;

#ifdef _WIN32
typedef unsigned long OSThreadProcedure(void* parameter);
#endif

OSThreadHandle os_thread_create(OSThreadProcedure* thread_procedure, void* parameter);
u32            os_thread_resume(OSThreadHandle thread_handle);
u32            os_thread_suspend(OSThreadHandle thread_handle);

#define H_OS_THREAD_H
#endif
