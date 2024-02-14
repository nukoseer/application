#ifndef H_OS_GRAPHICS_TYPES_H

#define OS_GRAPHICS_MAX_VERTEX_BUFFER_COUNT        (8)
#define OS_GRAPHICS_MAX_VERTEX_ATTRIBUTE_COUNT     (16)
#define OS_GRAPHICS_MAX_UNIFORM_MEMBER_COUNT       (16)
#define OS_GRAPHICS_MAX_SHADER_STAGE_UNIFORM_COUNT (4)
#define OS_GRAPHICS_MAX_SHADER_STAGE_COUNT         (2)

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
    const char* byte_code;
    memory_size byte_code_size;
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

typedef enum OSGraphicsBufferUsage
{
    OS_GRAPHICS_BUFFER_USAGE_DEFAULT,
    
    OS_GRAPHICS_BUFFER_USAGE_IMMUTABLE,
    OS_GRAPHICS_BUFFER_USAGE_DYNAMIC,

    OS_GRAPHICS_BUFFER_USAGE_COUNT,
} OSGraphicsBufferUsage;

typedef enum OSGraphicsBufferType
{
    OS_GRAPHICS_BUFFER_TYPE_DEFAULT_BUFFER,
    
    OS_GRAPHICS_BUFFER_TYPE_VERTEX_BUFFER,
    OS_GRAPHICS_BUFFER_TYPE_INDEX_BUFFER,

    OS_GRAPHICS_BUFFER_TYPE_COUNT,
} OSGraphicsBufferType;

typedef struct OSGraphicsBufferDesc
{
    OSGraphicsBufferUsage usage;
    OSGraphicsBufferType type;
    memory_size size;
    void* data;
} OSGraphicsBufferDesc;

#define H_OS_GRAPHICS_TYPES_H
#endif


