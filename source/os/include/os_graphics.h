#ifndef H_OS_GRAPHICS_H

#define OS_GRAPHICS_MAX_VERTEX_BUFFER_COUNT        (8)
#define OS_GRAPHICS_MAX_VERTEX_ATTRIBUTE_COUNT     (16)
#define OS_GRAPHICS_MAX_UNIFORM_MEMBER_COUNT       (16)
#define OS_GRAPHICS_MAX_SHADER_STAGE_UNIFORM_COUNT (4)

typedef uptr OSGraphicsShader;
typedef uptr OSGraphicsInputLayout;
typedef uptr OSGraphicsTexture2D;

typedef struct OSGrahicsShaderAttributeDesc
{
    const char* semantic_name;
    u32 semantic_index;
    PADDING(4);
} OSGrahicsShaderAttributeDesc;

typedef enum OSGraphicsUniformLayoutType
{
    OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_NULL,

    OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_FLOAT,
    OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_FLOAT2,
    OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_FLOAT3,
    OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_FLOAT4,

    OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_COUNT,
} OSGraphicsUniformLayoutType;

typedef struct OSGrahicsShaderUniformDesc
{
    const char* name;
    OSGraphicsUniformLayoutType layout_type;
    u32 count;
} OSGrahicsShaderUniformDesc;

typedef struct OSGrahicsShaderUniformBlockDesc
{
    memory_size size;
    OSGrahicsShaderUniformDesc uniforms[OS_GRAPHICS_MAX_UNIFORM_MEMBER_COUNT];
} OSGrahicsShaderUniformBlockDesc;

typedef struct OSGrahicsShaderStageDesc
{
    const char* source;
    OSGrahicsShaderUniformBlockDesc uniform_blocks[OS_GRAPHICS_MAX_SHADER_STAGE_UNIFORM_COUNT];
    // images;
    // samplers;
} OSGrahicsShaderStageDesc;

typedef struct OSGraphicsShaderDesc
{
    OSGrahicsShaderAttributeDesc attributes[OS_GRAPHICS_MAX_VERTEX_ATTRIBUTE_COUNT];
    OSGrahicsShaderStageDesc vertex_shader;
    OSGrahicsShaderStageDesc pixel_shader;
} OSGraphicsShaderDesc;

typedef enum OSGraphicsVertexFormatType
{
    OS_GRAPHICS_VERTEX_FORMAT_TYPE_NULL,
    
    OS_GRAPHICS_VERTEX_FORMAT_TYPE_FLOAT2,
    OS_GRAPHICS_VERTEX_FORMAT_TYPE_FLOAT3,
    OS_GRAPHICS_VERTEX_FORMAT_TYPE_FLOAT4,

    OS_GRAPHICS_VERTEX_FORMAT_TYPE_COUNT,
} OSGraphicsVertexFormatType;

typedef struct OSGraphicsVertexAttributeState
{
    u32 buffer_index;
    u32 offset;
    OSGraphicsVertexFormatType format;
} OSGraphicsVertexAttributeState;

typedef struct OSGraphicsVertexBufferState
{
    u32 stride;
} OSGraphicsVertexBufferState;

typedef struct OSGraphicsVertexState
{
    OSGraphicsVertexBufferState vertex_buffers[OS_GRAPHICS_MAX_VERTEX_BUFFER_COUNT];
    OSGraphicsVertexAttributeState attributes[OS_GRAPHICS_MAX_VERTEX_ATTRIBUTE_COUNT];
} OSGraphicsVertexBufferLayoutState;

void os_graphics_use_shader(OSWindow os_window, OSGraphicsShader shader);
void os_graphics_use_input_layout(OSWindow os_window, OSGraphicsInputLayout input_layout);
void os_graphics_set_vertex_buffer_data(OSWindow os_window, const void* vertex_buffer_data, u32 vertex_buffer_size);
void os_graphics_add_vertex_buffer_data(OSWindow os_window, const void* vertex_buffer_data, u32 vertex_buffer_size);
OSGraphicsInputLayout os_graphics_create_input_layout(const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                                      const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count);
OSGraphicsTexture2D os_graphics_create_texture_2D(OSWindow os_window, const u32* texture_buffer, i32 width, i32 height);
void os_graphics_use_texture_2Ds(OSWindow os_window, OSGraphicsTexture2D* texture_2Ds, u32 texture_count);
OSGraphicsShader os_graphics_create_vertex_shader(const u8* shader_buffer, u32 shader_buffer_size);
OSGraphicsShader os_graphics_create_pixel_shader(const u8* shader_buffer, u32 shader_buffer_size);
void os_graphics_clear(OSWindow os_window, RGBA color);
void os_graphics_draw_rectangle(OSWindow os_window, i32 x, i32 y, i32 width, i32 height, RGBA color);
void os_graphics_draw_circle_section(OSWindow os_window, i32 center_x, i32 center_y, f32 radius,
                                     f32 start_angle, f32 end_angle, i32 segments, RGBA color);
void os_graphics_draw_triangle(OSWindow os_window, V2 v1, V2 v2, V2 v3, RGBA color);
void os_graphics_draw_circle(OSWindow os_window, i32 center_x, i32 center_y, f32 radius, RGBA color);
void os_graphics_draw_pixel(OSWindow os_window, i32 x, i32 y, RGBA color);
void os_graphics_draw(OSWindow os_window);

#define H_OS_GRAPHICS_H
#endif
