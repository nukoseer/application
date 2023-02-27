#include "types.h"
#include "utils.h"

#include "os_window.h"
#include "os_graphics.h"

typedef void OSGraphicsSetVertexBufferData(uptr graphics, void* vertex_buffer_data, u32 vertex_buffer_size);
typedef void OSGraphicsSetVertexInputLayouts(uptr graphics, const char** names, u32* offsets, u32* formats, u32 stride, u32 layout_count);
typedef void OSGraphicsDraw(uptr graphics);

typedef struct OSGraphics
{
    OSGraphicsSetVertexBufferData* set_vertex_buffer_data;
    OSGraphicsSetVertexInputLayouts* set_vertex_input_layouts;
    OSGraphicsDraw* draw;
} OSGraphics;

#ifdef _WIN32
#include "win32_window.h"
#include "win32_graphics.h"

static OSGraphics os_graphics =
{
    .set_vertex_buffer_data = &win32_graphics_set_vertex_buffer_data,
    .set_vertex_input_layouts = &win32_graphics_set_vertex_input_layouts,
    .draw = &win32_graphics_draw,
};

#else
#error _WIN32 must be defined.
#endif

static uptr get_graphics_handle_from_window(OSWindowHandle os_window_handle)
{
    uptr graphics_handle = os_window_get_graphics_handle(os_window_handle);

    ASSERT(graphics_handle);

    return graphics_handle;
}

void os_graphics_set_vertex_buffer_data(OSWindowHandle os_window_handle, void* vertices, u32 vertex_count)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    ASSERT(os_graphics.set_vertex_buffer_data);

    if (graphics_handle)
    {
        os_graphics.set_vertex_buffer_data(graphics_handle, vertices, vertex_count);
    }
}

void os_graphics_set_vertex_input_layouts(OSWindowHandle os_window_handle,
                                          const char** names, u32* offsets, u32* formats, u32 stride, u32 layout_count)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    ASSERT(os_graphics.set_vertex_input_layouts);

    if (graphics_handle)
    {
        os_graphics.set_vertex_input_layouts(graphics_handle, names, offsets, formats, stride, layout_count);   
    }
}

void os_graphics_draw(OSWindowHandle os_window_handle)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    ASSERT(os_graphics.draw);

    if (graphics_handle)
    {
        os_graphics.draw(graphics_handle);    
    }
}
