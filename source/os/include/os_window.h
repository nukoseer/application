#ifndef H_OS_WINDOW_H

typedef memory_size OSWindowHandle;

typedef enum OSKey
{
    OS_KEY_NULL,
    
    OS_KEY_ESC,
    OS_KEY_0,
    OS_KEY_1,
    OS_KEY_2,
    OS_KEY_3,
    OS_KEY_4,
    OS_KEY_5,
    OS_KEY_6,
    OS_KEY_7,
    OS_KEY_8,
    OS_KEY_9,
    OS_KEY_A,
    OS_KEY_B,
    OS_KEY_C,
    OS_KEY_D,
    OS_KEY_E,
    OS_KEY_F,
    OS_KEY_G,
    OS_KEY_H,
    OS_KEY_I,
    OS_KEY_J,
    OS_KEY_K,
    OS_KEY_L,
    OS_KEY_M,
    OS_KEY_N,
    OS_KEY_O,
    OS_KEY_P,
    OS_KEY_Q,
    OS_KEY_R,
    OS_KEY_S,
    OS_KEY_T,
    OS_KEY_U,
    OS_KEY_V,
    OS_KEY_W,
    OS_KEY_X,
    OS_KEY_Y,
    OS_KEY_Z,
    OS_KEY_TAB,
    OS_KEY_SPACE,
    OS_KEY_ENTER,
    OS_KEY_CTRL,
    OS_KEY_SHIFT,
    OS_KEY_ALT,
    OS_KEY_UP,
    OS_KEY_LEFT,
    OS_KEY_DOWN,
    OS_KEY_RIGHT,
    OS_KEY_MOUSE_LEFT,
    OS_KEY_MOUSE_MIDDLE,
    OS_KEY_MOUSE_RIGHT,
    
    OS_KEY_COUNT
} OSKey;

typedef enum OSEventType
{
    OS_EVENT_TYPE_NULL,

    OS_EVENT_TYPE_WINDOW_CLOSE,
    OS_EVENT_TYPE_PRESS,
    OS_EVENT_TYPE_RELEASE,
    OS_EVENT_TYPE_MOUSE_SCROLL,

    OS_EVENT_TYPE_COUNT,
} OSEventType;

typedef enum OSEventModifier
{
    OS_MODIFIER_CTRL  = (1 << 0), 
    OS_MODIFIER_SHIFT = (1 << 1),
    OS_MODIFIER_ALT   = (1 << 2),
} OSEventModifier;

typedef struct OSEvent OSEvent;
struct OSEvent
{
    OSWindowHandle window_handle;
    OSEvent* next;
    OSEvent* prev;
    OSEventType type;
    OSEventModifier modifier;
    OSKey key;

    u32 padding;
};

typedef struct OSEventList
{
    OSEvent* first;
    OSEvent* last;
    u64 count;
} OSEventList;

OSWindowHandle os_window_open(const char* title, i32 width, i32 height);
void os_window_close(OSWindowHandle os_window_handle);
OSEventList os_window_get_events(void);
void os_window_init(void);

#define H_OS_WINDOW_H
#endif
