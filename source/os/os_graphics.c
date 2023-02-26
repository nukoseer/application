#include "types.h"
#include "utils.h"

#include "os_window.h"
#include "os_graphics.h"

typedef void OSGraphicsSetVertexBufferData(uptr graphics, void* vertex_buffer_data, u32 vertex_buffer_size);
typedef void OSGraphicsSetVertexInputLayouts(uptr graphics, const char** names, u32* offsets, u32* formats, u32 stride, u32 layout_count);

typedef struct OSGraphics
{
    OSGraphicsSetVertexBufferData* set_vertex_buffer_data;
    OSGraphicsSetVertexInputLayouts* set_vertex_input_layouts;
} OSGraphics;

#ifdef _WIN32
#include "win32_window.h"
#include "win32_graphics.h"

static OSGraphics os_graphics =
{
    .set_vertex_buffer_data = &win32_graphics_set_vertex_buffer_data,
    .set_vertex_input_layouts = &win32_graphics_set_vertex_input_layouts,
};

#else
#error _WIN32 must be defined.
#endif

static uptr get_graphics_handle_from_window(OSWindowHandle os_window_handle)
{
    uptr graphics_handle = win32_window_get_graphics_handle_from(os_window_handle);

    ASSERT(graphics_handle);

    return graphics_handle;
}

void os_graphics_set_vertex_buffer_data(OSWindowHandle os_window_handle, void* vertices, u32 vertex_count)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    ASSERT(os_graphics.set_vertex_buffer_data);
    os_graphics.set_vertex_buffer_data(graphics_handle, vertices, vertex_count);
}

void os_graphics_set_vertex_input_layouts(OSWindowHandle os_window_handle,
                                          const char** names, u32* offsets, u32* formats, u32 stride, u32 layout_count)
{
    uptr graphics_handle = get_graphics_handle_from_window(os_window_handle);

    ASSERT(os_graphics.set_vertex_input_layouts);
    os_graphics.set_vertex_input_layouts(graphics_handle, names, offsets, formats, stride, layout_count);
}
