#ifndef H_OS_GRAPHICS_TYPES_H

#define OS_GRAPHICS_MAX_VERTEX_BUFFER_COUNT        (8)
#define OS_GRAPHICS_MAX_VERTEX_ATTRIBUTE_COUNT     (16)
#define OS_GRAPHICS_MAX_UNIFORM_MEMBER_COUNT       (16)
#define OS_GRAPHICS_MAX_SHADER_STAGE_UNIFORM_COUNT (4)
#define OS_GRAPHICS_MAX_SHADER_STAGE_TEXTURE_COUNT (12)
#define OS_GRAPHICS_MAX_SHADER_STAGE_SAMPLER_COUNT (8)
#define OS_GRAPHICS_MAX_SHADER_STAGE_COUNT         (2)

typedef uptr OSGraphicsShader;
typedef uptr OSGraphicsBuffer;
typedef uptr OSGraphicsTexture;
typedef uptr OSGraphicsSampler;
typedef uptr OSGraphicsPipeline;
typedef uptr OSGraphicsInputLayout;
typedef uptr OSGraphicsTexture2D;

typedef enum OSGraphicsPrimitiveType
{
    OS_GRAPHICS_PRIMITIVE_TYPE_NULL,
    
    OS_GRAPHICS_PRIMITIVE_TYPE_LINES,
    OS_GRAPHICS_PRIMITIVE_TYPE_TRIANGLES,

    OS_GRAPHICS_PRIMITIVE_TYPE_COUNT,
} OSGraphicsPrimitiveType;

typedef enum OSGraphicsVertexFormatType
{
    OS_GRAPHICS_VERTEX_FORMAT_TYPE_NULL,
    
    OS_GRAPHICS_VERTEX_FORMAT_TYPE_FLOAT2,
    OS_GRAPHICS_VERTEX_FORMAT_TYPE_FLOAT3,
    OS_GRAPHICS_VERTEX_FORMAT_TYPE_FLOAT4,

    OS_GRAPHICS_VERTEX_FORMAT_TYPE_COUNT,
} OSGraphicsVertexFormatType;

typedef enum OSGraphicsUsageType
{
    OS_GRAPHICS_USAGE_TYPE_DEFAULT,
    
    OS_GRAPHICS_USAGE_TYPE_IMMUTABLE,
    OS_GRAPHICS_USAGE_TYPE_DYNAMIC,

    OS_GRAPHICS_USAGE_TYPE_COUNT,
} OSGraphicsUsageType;

typedef enum OSGraphicsBufferType
{
    OS_GRAPHICS_BUFFER_TYPE_DEFAULT_BUFFER,
    
    OS_GRAPHICS_BUFFER_TYPE_VERTEX_BUFFER,
    OS_GRAPHICS_BUFFER_TYPE_INDEX_BUFFER,

    OS_GRAPHICS_BUFFER_TYPE_COUNT,
} OSGraphicsBufferType;

typedef enum OSGraphicsTextureType
{
    OS_GRAPHICS_TEXTURE_TYPE_2D,
} OSGraphicsTextureType;

typedef enum OSGraphicsPixelFormat
{
    OS_GRAPHICS_PIXEL_FORMAT_NULL,

    OS_GRAPHICS_PIXEL_FORMAT_R8G8B8A8,

    OS_GRAPHICS_PIXEL_FORMAT_COUNT,
} OSGraphicsPixelFormat;

typedef enum OSGraphicsUniformLayoutType
{
    OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_NULL,
    OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_FLOAT,
    OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_FLOAT2,
    OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_FLOAT3,
    OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_FLOAT4,

    OS_GRAPHICS_UNIFORM_LAYOUT_TYPE_COUNT,
} OSGraphicsUniformLayoutType;

typedef enum OSGraphicsShaderStage
{
    OS_GRAPHICS_VERTEX_SHADER_STAGE,
    OS_GRAPHICS_PIXEL_SHADER_STAGE,
} OSGraphicsShaderStage;

typedef struct OSGraphicsShaderAttributeDesc
{
    const char* semantic_name;
    u32 semantic_index;
    PADDING(4);
} OSGraphicsShaderAttributeDesc;

typedef struct OSGraphicsShaderUniformDesc
{
    const char* name;
    OSGraphicsUniformLayoutType layout_type;
    u32 array_count;
} OSGraphicsShaderUniformDesc;

typedef struct OSGraphicsShaderUniformBlockDesc
{
    memory_size size;
    OSGraphicsShaderUniformDesc uniforms[OS_GRAPHICS_MAX_UNIFORM_MEMBER_COUNT];
} OSGraphicsShaderUniformBlockDesc;

typedef struct OSGraphicsShaderTextureDesc
{
    b32 used;
    OSGraphicsTextureType texture_type;
} OSGraphicsShaderTextureDesc;

typedef struct OSGraphicsShaderStageDesc
{
    const char* source;
    const char* byte_code;
    memory_size byte_code_size;
    OSGraphicsShaderUniformBlockDesc uniform_blocks[OS_GRAPHICS_MAX_SHADER_STAGE_UNIFORM_COUNT];
    OSGraphicsShaderTextureDesc textures[OS_GRAPHICS_MAX_SHADER_STAGE_TEXTURE_COUNT];
    // samplers;
} OSGraphicsShaderStageDesc;

typedef struct OSGraphicsShaderDesc
{
    OSGraphicsShaderAttributeDesc attributes[OS_GRAPHICS_MAX_VERTEX_ATTRIBUTE_COUNT];
    OSGraphicsShaderStageDesc vertex_shader;
    OSGraphicsShaderStageDesc pixel_shader;
} OSGraphicsShaderDesc;

typedef struct OSGraphicsVertexAttributeState
{
    u32 buffer_index;
    u32 offset;
    OSGraphicsVertexFormatType format;
} OSGraphicsVertexAttributeState;

typedef struct OSGraphicsVertexBufferLayoutState
{
    u32 stride;
} OSGraphicsVertexBufferLayoutState;

typedef struct OSGraphicsVertexLayoutState
{
    OSGraphicsVertexBufferLayoutState vertex_buffer_layouts[OS_GRAPHICS_MAX_VERTEX_BUFFER_COUNT];
    OSGraphicsVertexAttributeState attributes[OS_GRAPHICS_MAX_VERTEX_ATTRIBUTE_COUNT];
} OSGraphicsVertexLayoutState;

typedef struct OSGraphicsBufferDesc
{
    OSGraphicsUsageType usage;
    OSGraphicsBufferType type;
    memory_size size;
    void* data;
} OSGraphicsBufferDesc;

typedef enum OSGraphicsFilterType
{
    OS_GRAPHICS_FILTER_TYPE_NONE,
    OS_GRAPHICS_FILTER_TYPE_NEAREST,
    OS_GRAPHICS_FILTER_TYPE_LINEAR,

    OS_GRAPHICS_FILTER_TYPE_COUNT,
} OSGraphicsFilterType;

typedef enum OSGraphicsWrapType
{
    OS_GRAPHICS_WRAP_TYPE_REPEAT,
    OS_GRAPHICS_WRAP_TYPE_CLAMP_TO_EDGE,
    OS_GRAPHICS_WRAP_TYPE_CLAMP_TO_BORDER,
    OS_GRAPHICS_WRAP_TYPE_MIRRORED_REPEAT,
    
    OS_GRAPHICS_WRAP_TYPE_COUNT,
} OSGraphicsWrapType;

typedef struct OSGraphicsSamplerDesc
{
    OSGraphicsFilterType min_filter;
    OSGraphicsFilterType mag_filter;
    OSGraphicsFilterType mipmap_filter;
    OSGraphicsWrapType wrap_u;
    OSGraphicsWrapType wrap_v;
    OSGraphicsWrapType wrap_w;
} OSGraphicsSamplerDesc;

typedef struct OSGraphicsTextureDesc
{
    OSGraphicsTextureType type;
    OSGraphicsUsageType usage;
    OSGraphicsPixelFormat format;
    // TODO: What we should do if we want to use a texture as a render target? This is a place holder.
    b32 render_target;
    i32 width;
    i32 height;
    // TODO: Consider sample count, mip maps, etc.
    void* data;
    memory_size size;
} OSGraphicsTextureDesc;

typedef struct OSGraphicsPipelineDesc
{
    OSGraphicsShader shader;
    OSGraphicsVertexLayoutState vertex_layout;
    OSGraphicsPrimitiveType primitive_type;
    PADDING(4);
    // TODO: Depth, stencil things?
} OSGraphicsPipelineDesc;

typedef struct OSGraphicsStageBindings
{
    OSGraphicsTexture textures[OS_GRAPHICS_MAX_SHADER_STAGE_TEXTURE_COUNT];
    OSGraphicsSampler samplers[OS_GRAPHICS_MAX_SHADER_STAGE_SAMPLER_COUNT];
} OSGraphicsStageBindings;

typedef struct OSGraphicsBindings
{
    OSGraphicsStageBindings vertex_shader;
    OSGraphicsStageBindings pixel_shader;
    OSGraphicsBuffer vertex_buffers[OS_GRAPHICS_MAX_VERTEX_BUFFER_COUNT];
    OSGraphicsBuffer index_buffer;
    u32 vertex_buffer_offsets[OS_GRAPHICS_MAX_VERTEX_BUFFER_COUNT];
    u32 index_buffer_offset;
    PADDING(4);
} OSGraphicsBindings;

#define H_OS_GRAPHICS_TYPES_H
#endif


