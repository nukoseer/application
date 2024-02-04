#ifndef H_UTILS_H

#include "types.h"
#include "os_memory.h"

//////////////////////////////////
// NOTE: General helper macros. //
//////////////////////////////////
#if 1
    #include "os_log.h"
    #define ASSERT(x) do { if (!(x)) { OS_LOG_FATAL("ASSERT"); *(volatile int*)0; } } while (0)
#else
    #define ASSERT(x)
#endif

#define STRINGFY_(x) #x
#define STRINGFY(x) STRINGFY_(x)

#define CONCAT_(x, y) x##y
#define CONCAT(x, y) CONCAT_(x, y)

#define UNIQUE(name) CONCAT(name, __LINE__)

#define UNUSED_VARIABLE(x)  ((void)(x))
#define ARRAY_COUNT(x)      (sizeof(x) / sizeof(*(x)))
#define PADDING(x)          u8 CONCAT(padding, __LINE__)[x]

#define MAX(a, b)           ((a) >= (b) ? (a) : (b))
#define OFFSETOF(T, m)      ((size_t)(&(((T*)0)->m)))

#define KILOBYTES(n) ((n) << 10)
#define MEGABYTES(n) (KILOBYTES(n) << 10)
#define GIGABYTES(n) (MEGABYTES(n) << 10)

#define U32_MAX (0xFFFFFFFFU)

#define DEFER_LOOP(begin, end) for (u32 UNIQUE(_i_) = ((begin), 0); UNIQUE(_i_) == 0; (++UNIQUE(_i_), (end)))
#define DEFER(x) DEFER_LOOP(0, x)

inline u32 string_length(const char* str)
{
    u32 length = 0;

    while (str && *str)
    {
        ++length;
        ++str;
    }

    return length;
}

///////////////////////////////////////////////////
// NOTE: Start - stretchy buffer implementation. //
///////////////////////////////////////////////////
typedef struct BufferHeader
{
    i64 len;
    i64 cap;
    u8 buf[]; // NOTE: MSVC supports flexible array members as a compiler extension. But, normally it is in the C99 standard?.
} BufferHeader;

// NOTE: Internal macros.
#define BUFF__BASE(buff) ((BufferHeader*)(void*)((u8*)(buff) - OFFSETOF(BufferHeader, buf)))
#define BUFF__LENGTH(buff) ((buff) ? BUFF__BASE(buff)->len : 0)
#define BUFF__CAPACITY(buff) ((buff) ? BUFF__BASE(buff)->cap : 0)
#define BUFF__SHOULD_GROW(buff, n) (BUFF__LENGTH(buff) + (n) <= BUFF__CAPACITY(buff))
#define BUFF__GROW(buff, n) (BUFF__SHOULD_GROW(buff, n) ? 0 : ((buff) = buffer__grow(buff, (n) + BUFF__LENGTH(buff), sizeof(*(buff)))))

// NOTE: User macros.
#define BUFFER_LENGTH(buff) BUFF__LENGTH(buff)
#define BUFFER_CAPACITY(buff) BUFF__CAPACITY(buff)
#define BUFFER_PUSH(buff, x) (BUFF__GROW(buff, 1), (buff)[BUFF__BASE(buff)->len++] = (x))
#define BUFFER_FREE(buff) ((buff) ? (os_memory_heap_release(BUFF__BASE(buff)), (buff) = NULL) : 0)

inline void* buffer__grow(void* buffer, i64 new_capacity, i64 element_size)
{
    i64 capacity = MAX(1 + BUFFER_CAPACITY(buffer) * 2, new_capacity);
    i64 size = capacity * element_size + (i64)OFFSETOF(BufferHeader, buf);
    BufferHeader* buffer_header = NULL;

    ASSERT(capacity >= new_capacity);

    if (!buffer)
    {
        buffer_header = (BufferHeader*)os_memory_heap_allocate((memory_size)size, FALSE);
        buffer_header->len = 0;
    }
    else
    {
        buffer_header = (BufferHeader*)os_memory_heap_reallocate(BUFF__BASE(buffer), (memory_size)size, FALSE);
    }

    buffer_header->cap = capacity;

    return buffer_header->buf;
}
////////////////////////////////////////////////
// NOTE: End - stretcy buffer implementation. //
////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// NOTE: Start - generic doubly linked list (DLL) macro implementation. //
//////////////////////////////////////////////////////////////////////////
// TODO: More tests.
#define DLL_INSERT(f, l, p, e, prev, next)                                               \
(!(f) ? (((f) = (l) = (e)), ((e)->next = (e)->prev = 0)) :                               \
(!(p) ? ((e)->prev = 0, (e)->next = (f), (f) ? ((f)->prev = (e), ((f) = (e))) : 0) :     \
((p)->next ? (p)->next->prev = (e) : 0, ((e)->next = (p)->next, (e)->prev = (p), (p)->next = (e)), ((p == l) ? ((l) = (e)) : 0))))

#define DLL_PUSH_BACK(first, last, element) DLL_INSERT(first, last, last, element, prev, next)
#define DLL_PUSH_FRONT(first, last, element) DLL_INSERT(last, first, first, element, next, prev)

#define FOREACH_BY_NAME_(sentinel, name) for (name = (sentinel); name != 0; name = (name)->next)
#define FOREACH_BY_NAME(sentinel, name) FOREACH_BY_NAME_(sentinel, name)
#define FOREACH_(sentinel, type, name) for (type* name = (sentinel); name != 0; name = (name)->next)
#define FOREACH(sentinel, type, name) FOREACH_(sentinel, type, name)

// NOTE: Function version of DLL_INSERT macro. (f=first, l=last, p=prev, e=element)
// void DLL_INSERT(T** first, T** last, T* prev, T* element)       \
// {                                                               \
//     if (!(*first))                                              \
//     {                                                           \
//         *first = *last = element;                               \
//         element->next = 0;                                      \
//         element->prev = 0;                                      \
//     }                                                           \
//     else if (!prev)                                             \
//     {                                                           \
//         element->prev = 0;                                      \
//         element->next = *first;                                 \
//                                                                 \
//         if (*first)                                             \
//         {                                                       \
//             (*first)->prev = element;                           \
//             *first = element;                                   \
//         }                                                       \
//     }                                                           \
//     else                                                        \
//     {                                                           \
//         if (prev->next)                                         \
//         {                                                       \
//             prev->next->prev = element;                         \
//         }                                                       \
//                                                                 \
//         element->next = prev->next;                             \
//         element->prev = prev;                                   \
//         prev->next = element;                                   \
//                                                                 \
//         if (prev == *last)                                      \
//         {                                                       \
//             *last = element;                                    \
//         }                                                       \
//     }                                                           \
// }
////////////////////////////////////////////////////////////////////////
// NOTE: End - generic doubly linked list (DLL) macro implementation. //
////////////////////////////////////////////////////////////////////////

#define H_UTILS_H
#endif
