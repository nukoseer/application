// NOTE: Resource: https://gist.github.com/mmozeiko/5e727f845db182d468a34d524508ad5f

#define COBJMACROS

#include <Windows.h>

#pragma warning(push, 0)
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>
#pragma warning(pop)

#include "types.h"
#include "utils.h"
#include "memory_utils.h"
#include "maths.h"

#include "os_graphics_types.h"

#include "win32_memory.h"
#include "win32_window.h"
#include "win32_graphics.h"

#define VERTEX_BUFFER_SIZE MEGABYTES(1)

#define PUSH_OR_GET_FROM_FREE_LIST(state, arena, name, element)         \
    do {                                                                \
        if ((state)->name##_free_list)                                  \
        {                                                               \
            element = (state)->name##_free_list;                        \
            (state)->name##_free_list = (state)->name##_free_list->next; \
            STRUCT_ZERO(element, sizeof(*(element)));                   \
        }                                                               \
        else                                                            \
        {                                                               \
            element = PUSH_SIZE(arena, sizeof(*(element)));             \
        }                                                               \
    } while(0)

// TODO: MOST OF THE FUNCTIONS IN THIS FILE DO NOT RETURN ANYTHING.
// IS IT A GOOD IDEA TO LEAVE THEM AS THEY ARE OR SHOULD THEY RETURN
// SOMETHING FOR ERROR CHECKING/ASSERTING?

typedef struct GraphicsBuffer
{
    memory_size size;
    OSGraphicsBufferType type;
    OSGraphicsUsageType usage;
} GraphicsBuffer;

typedef struct Win32GraphicsBuffer
{
    GraphicsBuffer buffer;

    // NOTE: D3D11 specific.
    struct
    {
        ID3D11Buffer* buffer;  
    } d3d11;
    
    struct Win32GraphicsBuffer* next;
} Win32GraphicsBuffer;

typedef struct GraphicsTexture
{
    OSGraphicsTextureType type;
    OSGraphicsUsageType usage;
    OSGraphicsPixelFormat format;
    b32 render_target;
    i32 width;
    i32 height;
} GraphicsTexture;

typedef struct Win32GraphicsTexture
{
    GraphicsTexture texture;

    // NOTE: D3D11 specific.
    struct
    {
        ID3D11Resource* resource;
        ID3D11Texture2D* texture_2D;
        ID3D11ShaderResourceView* shader_resource_view;
        DXGI_FORMAT format;
        PADDING(4);
    } d3d11;

    struct Win32GraphicsTexture* next;
} Win32GraphicsTexture;

typedef enum Win32GraphicsShaderType
{
    WIN32_GRAPHICS_SHADER_TYPE_VERTEX,
    WIN32_GRAPHICS_SHADER_TYPE_PIXEL,

    WIN32_GRAPHICS_SHADER_TYPE_COUNT,
} Win32GraphicsShaderType;

typedef struct Win32GraphicsVertexShader
{
    ID3D11VertexShader* shader;
    ID3D11Buffer* buffer;
} Win32GraphicsVertexShader;

typedef struct Win32GraphicsPixelShader
{
    ID3D11PixelShader* shader;
    // ID3D11ShaderResourceView* texture_view[2];
} Win32GraphicsPixelShader;

typedef struct Win32GraphicsShader
{
    Win32GraphicsShaderType type;
    PADDING(4);
    union
    {
        Win32GraphicsVertexShader vertex_shader;
        Win32GraphicsPixelShader pixel_shader;   
    };
} Win32GraphicsShader;

typedef struct Win32GraphicsTexture2D
{
    ID3D11Texture2D* texture;
    ID3D11ShaderResourceView* view;
    i32 width;
    i32 height;
} Win32GraphicsTexture2D;

typedef struct Win32GraphicsShaderAttribute
{
    char semantic_name[16];
    u32 semantic_index;
} Win32GraphicsShaderAttribute;

typedef struct Win32GraphicsShaderStage
{
    ID3D11Buffer* constant_buffers[OS_GRAPHICS_MAX_SHADER_STAGE_UNIFORM_COUNT];
} Win32GraphicsShaderStage;

// TODO: If GraphicsXXX structs are only for validation maybe we can remove them?
typedef struct GraphicsUniformBlock
{
    memory_size size;
} GraphicsUniformBlock;

typedef struct GraphicsShaderTexture
{
    OSGraphicsTextureType texture_type;
} GraphicsShaderTexture;

typedef struct GraphicsShaderStage
{
    GraphicsUniformBlock uniform_blocks[OS_GRAPHICS_MAX_SHADER_STAGE_UNIFORM_COUNT];
    GraphicsShaderTexture textures[OS_GRAPHICS_MAX_SHADER_STAGE_UNIFORM_COUNT];
    u32 uniform_block_count;
    u32 texture_count;
} GraphicsShaderStage;

typedef struct GraphicsShader
{
    GraphicsShaderStage stage[OS_GRAPHICS_MAX_SHADER_STAGE_COUNT];
} GraphicsShader;

typedef struct _Win32GraphicsShader
{
    // NOTE: Common.
    GraphicsShader shader;
    
    // NOTE: D3D11 specific.
    struct
    {
        Win32GraphicsShaderAttribute attributes[OS_GRAPHICS_MAX_VERTEX_ATTRIBUTE_COUNT];
        Win32GraphicsShaderStage stages[OS_GRAPHICS_MAX_SHADER_STAGE_COUNT];
        ID3D11VertexShader* vertex_shader;
        ID3D11PixelShader* pixel_shader;
        const void* vertex_shader_data;
        memory_size vertex_shader_data_size;
    } d3d11;

    struct _Win32GraphicsShader* next;
} _Win32GraphicsShader;

typedef struct Win32GraphicsState
{
    MemoryArena* arena;
    _Win32GraphicsShader* shader_free_list;
    Win32GraphicsBuffer* buffer_free_list;
    Win32GraphicsTexture* texture_free_list;
} Win32GraphicsState;

// TODO: One input layout can be used with multiple vertex shaders that have same layout?
// Should we associate input layout and vertex shader or input layout and vertex buffer or
// it is enough to create an input layout and use it with particular vertex shader when it is needed. 
typedef struct Win32GraphicsInputLayout
{
    ID3D11InputLayout* layout;
    D3D11_INPUT_ELEMENT_DESC descs[16];
    u32 desc_count;
    // TODO: stride belongs here?
    u32 stride;
} Win32GraphicsInputLayout;

// TODO: We should check the alignment and padding of this struct.
typedef struct Win32Graphics
{
    HWND handle;
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    ID3D11Buffer* screen_buffer;
    ID3D11ShaderResourceView* texture_views[16];
    u32 texture_count;
    PADDING(4);
    ID3D11SamplerState* sampler_state;
    ID3D11BlendState* blend_state;
    ID3D11RasterizerState* rasterizer_state;
    ID3D11DepthStencilState* depth_state;
    ID3D11RenderTargetView* render_target_view;
    ID3D11DepthStencilView* depth_stencil_view;
    IDXGISwapChain1* swap_chain;
    i32 swap_chain_width;
    i32 swap_chain_height;
    // TODO: User should pass vertex_buffer_data?
    void* vertex_buffer_data;
    u32 vertex_buffer_size;
    FLOAT clear_color[4];
    PADDING(4);
    Win32GraphicsShader* shaders[WIN32_GRAPHICS_SHADER_TYPE_COUNT];
    Win32GraphicsInputLayout* input_layout;
} Win32Graphics;

static Win32GraphicsState* _graphics_state;
static ID3D11Device* graphics_device;
static ID3D11DeviceContext* graphics_context;

static Win32GraphicsState* get_graphics_state(void)
{
    return _graphics_state;
}

static void create_device_and_context(Win32Graphics* graphics)
{
    if (!graphics_device)
    {
        D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
        UINT flags = 0;
        HRESULT hresult = 0;

        // NOTE: Create D3D11 device & context.
#ifndef NDEBUG
        // NOTE: This enables VERY USEFUL debug messages in debugger output.
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        hresult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
                                    flags, levels, ARRAY_COUNT(levels),
                                    D3D11_SDK_VERSION, &graphics_device, NULL, &graphics_context);
        // NOTE: Make sure device creation succeeeds before continuing
        // for simple application you could retry device creation with
        // D3D_DRIVER_TYPE_WARP driver type which enables software
        // rendering (could be useful on broken drivers or remote
        // desktop situations).
        ASSERT(SUCCEEDED(hresult));

#ifndef NDEBUG
        // NOTE: For debug builds enable VERY USEFUL debug break on API errors.
        {
            ID3D11InfoQueue* info = 0;

            ID3D11Device_QueryInterface(graphics_device, &IID_ID3D11InfoQueue, (void**)&info);
            ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
            ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
            ID3D11InfoQueue_Release(info);
        }
        // NOTE: After this there is no need to check for any errors on
        // device functions manually so all HRESULT return values in this
        // code will be ignored debugger will break on errors anyway.
#endif
    }

    graphics->device = graphics_device;
    graphics->context = graphics_context;
}

static void create_swap_chain(Win32Graphics* graphics)
{
    ID3D11Device* device = graphics->device;
    IDXGISwapChain1** swap_chain = &graphics->swap_chain;
    HWND handle = graphics->handle;
    IDXGIFactory2* factory = 0;
    DXGI_SWAP_CHAIN_DESC1 desc = { 0 };
    D3D11_BUFFER_DESC screen_desc = { 0 };
    ID3D11Buffer** screen_buffer = &graphics->screen_buffer;
    HRESULT hresult = 0;
    i32 x, y, width, height = 0;

    win32_window_get_position_and_size(win32_window_get_window_from((uptr)graphics->handle), &x, &y, &width, &height);

    graphics->swap_chain_width = width;
    graphics->swap_chain_height = height;

    screen_desc = (D3D11_BUFFER_DESC)
    {
        .ByteWidth = 4 * sizeof(f32), // TODO: D3D11 says it must be multiple of 16?
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };
    ID3D11Device_CreateBuffer(device, &screen_desc, 0, screen_buffer);

    // NOTE: Create DXGI 1.2 factory for creating swap chain.
    hresult = CreateDXGIFactory(&IID_IDXGIFactory2, (void**)&factory);
    ASSERT(SUCCEEDED(hresult));

    // NOTE: Default 0 value for width & height
    // means to get it from HWND automatically
    // desc.Width = 0,
    // desc.Height = 0,
    // or use DXGI_FORMAT_R8G8B8A8_UNORM_SRGB for storing sRGB.
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // NOTE: FLIP presentation model does not allow
    // MSAA framebuffer if you want MSAA then you'll
    // need to render offscreen and manually resolve
    // to non-MSAA framebuffer.
    desc.SampleDesc = (DXGI_SAMPLE_DESC){ .Count = 1, .Quality = 0 };

    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;

    // NOTE: We don't want any automatic scaling of
    // window content this is supported only on FLIP
    // presentation model.
    desc.Scaling = DXGI_SCALING_NONE;

    // NOTE: Use more efficient FLIP presentation model
    // Windows 10 allows to use DXGI_SWAP_EFFECT_FLIP_DISCARD
    // for Windows 8 compatibility use DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
    // for Windows 7 compatibility use DXGI_SWAP_EFFECT_DISCARD.
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    // NOTE: Create DXGI swap chain.
    hresult = IDXGIFactory2_CreateSwapChainForHwnd(factory, (IUnknown*)device, handle, &desc, NULL, NULL, swap_chain);

    // NOTE: Make sure swap chain creation succeeds before continuing.
    ASSERT(SUCCEEDED(hresult));
    IDXGIFactory_Release(factory);

    // NOTE: Disable Alt + Enter changing monitor resolution to match window size.
    IDXGISwapChain1_GetParent(*swap_chain, &IID_IDXGIFactory, (void**)&factory);
    IDXGIFactory_MakeWindowAssociation(factory, handle, DXGI_MWA_NO_ALT_ENTER);
    IDXGIFactory_Release(factory);
}

// TODO: Creating vertex buffer should be coupled with vertex shader?
// Maybe it is okay to have 1(?) vertex buffer per vertex shader but
// probably we also need independent way to create buffer.
static void create_vertex_buffer(Win32GraphicsVertexShader* vertex_shader)
{
    D3D11_BUFFER_DESC desc =
    {
        .ByteWidth = MEGABYTES(10), // NOTE: Max 10 megabytes?
        .Usage = D3D11_USAGE_DYNAMIC,
        .BindFlags = D3D11_BIND_VERTEX_BUFFER,
        .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
    };
    // D3D11_SUBRESOURCE_DATA initial_data = { .pSysMem = vertex_data };

    // ID3D11Device_CreateBuffer(device, &desc, &initial_data, vertex_buffer);
    ID3D11Device_CreateBuffer(graphics_device, &desc, 0, &vertex_shader->buffer);
}

static void set_vertex_input_layout(Win32GraphicsInputLayout* input_layout, const char* name, u32 offset, u32 format)
{
    DXGI_FORMAT dxgi_format = 0;

    switch (format)
    {
        case 1:
        {
            dxgi_format = DXGI_FORMAT_R32_FLOAT;
        }
        break;
        case 2:
        {
            dxgi_format = DXGI_FORMAT_R32G32_FLOAT;
        }
        break;
        case 3:
        {
            dxgi_format = DXGI_FORMAT_R32G32B32_FLOAT;
        }
        break;
        case 4:
        {
            dxgi_format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
        break;
    }

    input_layout->descs[input_layout->desc_count++] = (D3D11_INPUT_ELEMENT_DESC){ name, 0, dxgi_format, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };
}

static void create_sampler_state(Win32Graphics* graphics)
{
    ID3D11Device* device = graphics->device;
    ID3D11SamplerState** sampler_state = &graphics->sampler_state;

    // NOTE: Sampler state.
    {
        D3D11_SAMPLER_DESC desc =
        {
            .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
            .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
            .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
            .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
        };

        ID3D11Device_CreateSamplerState(device, &desc, sampler_state);
    }
}

static void create_blend_state(Win32Graphics* graphics)
{
    ID3D11Device* device = graphics->device;
    ID3D11BlendState** blend_state = &graphics->blend_state;

    // NOTE: Blend state.
    {
        // NOTE: Enable alpha blending.
        D3D11_BLEND_DESC desc =
        {
            .RenderTarget[0] =
            {
                .BlendEnable = TRUE,
                .SrcBlend = D3D11_BLEND_SRC_ALPHA,
                .DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
                .BlendOp = D3D11_BLEND_OP_ADD,
                .SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA,
                .DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA,
                .BlendOpAlpha = D3D11_BLEND_OP_ADD,
                .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
            },
        };
        ID3D11Device_CreateBlendState(device, &desc, blend_state);
    }
}

static void create_rasterizer_state(Win32Graphics* graphics)
{
    ID3D11Device* device = graphics->device;
    ID3D11RasterizerState** rasterizer_state = &graphics->rasterizer_state;

    // NOTE: Rasterizer state.
    {
        // NOTE: Disable culling.
        D3D11_RASTERIZER_DESC desc =
        {
            .FillMode = D3D11_FILL_SOLID,
            .CullMode = D3D11_CULL_NONE,
        };
        ID3D11Device_CreateRasterizerState(device, &desc, rasterizer_state);
    }
}

static void create_depth_state(Win32Graphics* graphics)
{
    ID3D11Device* device = graphics->device;
    ID3D11DepthStencilState** depth_state = &graphics->depth_state;

    // NOTE: Depth state.
    {
        // NOTE: Disable depth & stencil test.
        D3D11_DEPTH_STENCIL_DESC desc =
        {
            .DepthEnable = FALSE,
            .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
            .DepthFunc = D3D11_COMPARISON_LESS,
            .StencilEnable = FALSE,
            .StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK,
            .StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK,
            // .FrontFace = ...
            // .BackFace = ...
        };
        ID3D11Device_CreateDepthStencilState(device, &desc, depth_state);
    }
}

static void resize_swap_chain(Win32Graphics* graphics)
{
    i32 x, y, width, height = 0;

    win32_window_get_position_and_size(win32_window_get_window_from((uptr)graphics->handle), &x, &y, &width, &height);

    if (graphics->render_target_view == NULL ||
        graphics->swap_chain_width != width || graphics->swap_chain_height != height)
    {
        ID3D11Device* device = graphics->device;
        HRESULT hresult = { 0 };

        if (graphics->render_target_view)
        {
            // NOTE: Release old swap chain buffers.
            ID3D11DeviceContext_ClearState(graphics->context);
            ID3D11RenderTargetView_Release(graphics->render_target_view);
            ID3D11DepthStencilView_Release(graphics->depth_stencil_view);
            graphics->render_target_view = NULL;
        }

        // NOTE: Resize to new size for non-zero size.
        if (width != 0 && height != 0)
        {
            ID3D11Texture2D* back_buffer = 0;

            hresult = IDXGISwapChain1_ResizeBuffers(graphics->swap_chain, 0, width, height, DXGI_FORMAT_UNKNOWN, 0);

            if (FAILED(hresult))
            {
                ASSERT((char*)"Failed to resize swap chain!");
            }

            graphics->swap_chain_width = width;
            graphics->swap_chain_height = height;

            // NOTE: Create RenderTarget view for new backbuffer texture.
            IDXGISwapChain1_GetBuffer(graphics->swap_chain, 0, &IID_ID3D11Texture2D, (void**)&back_buffer);
            ID3D11Device_CreateRenderTargetView(device, (ID3D11Resource*)back_buffer, NULL, &graphics->render_target_view);
            ID3D11Texture2D_Release(back_buffer);

            {
                ID3D11Texture2D* depth = 0;
                D3D11_TEXTURE2D_DESC depth_desc =
                {
                    .Width = (UINT)width,
                    .Height = (UINT)height,
                    .MipLevels = 1,
                    .ArraySize = 1,
                    .Format = DXGI_FORMAT_D32_FLOAT, // or use DXGI_FORMAT_D32_FLOAT_S8X24_UINT if you need stencil.
                    .SampleDesc = { 1, 0 },
                    .Usage = D3D11_USAGE_DEFAULT,
                    .BindFlags = D3D11_BIND_DEPTH_STENCIL,
                };

                // NOTE: Create new depth stencil texture & DepthStencil view.
                ID3D11Device_CreateTexture2D(device, &depth_desc, NULL, &depth);
                ID3D11Device_CreateDepthStencilView(device, (ID3D11Resource*)depth, NULL, &graphics->depth_stencil_view);
                ID3D11Texture2D_Release(depth);
            }
        }
    }
}

static void clear_vertex_buffer_data(Win32Graphics* graphics)
{
    // TODO: Not sure to do this?
    graphics->vertex_buffer_size = 0;
}

static void clear_screen(Win32Graphics* graphics)
{
    ID3D11DeviceContext_ClearRenderTargetView(graphics->context, graphics->render_target_view, graphics->clear_color);
    ID3D11DeviceContext_ClearDepthStencilView(graphics->context, graphics->depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

static void draw(Win32Graphics* graphics)
{
    // NOTE: Can render only if window size is non-zero - we must have backbuffer & RenderTarget view created.
    if (graphics->render_target_view)
    {
        ID3D11DeviceContext* context = graphics->context;
        ID3D11SamplerState* sampler_state = graphics->sampler_state;
        ID3D11BlendState* blend_state = graphics->blend_state;
        ID3D11RasterizerState* rasterizer_state = graphics->rasterizer_state;
        ID3D11DepthStencilState* depth_state = graphics->depth_state;
        IDXGISwapChain1* swap_chain = graphics->swap_chain;
        ID3D11InputLayout* input_layout = graphics->input_layout->layout;
        ID3D11VertexShader* vertex_shader = graphics->shaders[WIN32_GRAPHICS_SHADER_TYPE_VERTEX]->vertex_shader.shader;
        ID3D11PixelShader* pixel_shader = graphics->shaders[WIN32_GRAPHICS_SHADER_TYPE_PIXEL]->pixel_shader.shader;
        ID3D11Buffer** vertex_buffer = &graphics->shaders[WIN32_GRAPHICS_SHADER_TYPE_VERTEX]->vertex_shader.buffer;
        ID3D11Buffer** screen_buffer = &graphics->screen_buffer;
        ID3D11ShaderResourceView** texture_views = graphics->texture_views;
        u32 texture_count = graphics->texture_count;
        D3D11_VIEWPORT viewport;
        D3D11_MAPPED_SUBRESOURCE mapped;

        // NOTE: Output viewport covering all client area of window.
        viewport = (D3D11_VIEWPORT)
        {
            .TopLeftX = 0,
            .TopLeftY = 0,
            .Width = (FLOAT)graphics->swap_chain_width,
            .Height = (FLOAT)graphics->swap_chain_height,
            .MinDepth = 0,
            .MaxDepth = 1,
        };

        // NOTE: Clear screen.
        clear_screen(graphics);

        // NOTE: Input Assembler.
        ID3D11DeviceContext_IASetInputLayout(context, input_layout);
        // TODO: Parameterize input layout topology?
        ID3D11DeviceContext_IASetPrimitiveTopology(context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        {
            UINT stride = graphics->input_layout->stride;
            UINT offset = 0;
            f32 screen[] = { 1.0f / (f32)graphics->swap_chain_width, 1.0f / (f32)graphics->swap_chain_height, 0.0f, 0.0f };

            ID3D11DeviceContext_Map(context, (ID3D11Resource*)*screen_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
            memcpy(mapped.pData, screen, sizeof(screen));
            ID3D11DeviceContext_Unmap(context, (ID3D11Resource*)*screen_buffer, 0);

            ID3D11DeviceContext_Map(context, (ID3D11Resource*)*vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
            memcpy(mapped.pData, graphics->vertex_buffer_data, graphics->vertex_buffer_size);
            ID3D11DeviceContext_Unmap(context, (ID3D11Resource*)*vertex_buffer, 0);

            ID3D11DeviceContext_IASetVertexBuffers(context, 0, 1, vertex_buffer, &stride, &offset);
        }

        // NOTE: Vertex Shader.
        ID3D11DeviceContext_VSSetConstantBuffers(context, 0, 1, screen_buffer);
        ID3D11DeviceContext_VSSetShader(context, vertex_shader, NULL, 0);

        // NOTE: Rasterizer Stage.
        ID3D11DeviceContext_RSSetViewports(context, 1, &viewport);
        ID3D11DeviceContext_RSSetState(context, rasterizer_state);

        // NOTE: Pixel Shader.
        ID3D11DeviceContext_PSSetSamplers(context, 0, 1, &sampler_state);
        ID3D11DeviceContext_PSSetShaderResources(context, 0, (u32)texture_count, texture_views);
        ID3D11DeviceContext_PSSetShader(context, pixel_shader, NULL, 0);

        // NOTE: Output Merger.
        ID3D11DeviceContext_OMSetBlendState(context, blend_state, NULL, ~0U);
        ID3D11DeviceContext_OMSetDepthStencilState(context, depth_state, 0);
        ID3D11DeviceContext_OMSetRenderTargets(context, 1, &graphics->render_target_view, graphics->depth_stencil_view);

        // NOTE: Draw 3 vertices.
        ID3D11DeviceContext_Draw(context, graphics->vertex_buffer_size / graphics->input_layout->stride, 0);
        IDXGISwapChain1_Present(swap_chain, 0, 0);

        // clear_vertex_buffer_data(graphics);
    }
}

static void add_vertex_data(Win32Graphics* graphics, const u8* vertex_buffer_data, u32 vertex_buffer_size)
{
    ASSERT(graphics->vertex_buffer_size + vertex_buffer_size < VERTEX_BUFFER_SIZE);
    
    memcpy((u8*)graphics->vertex_buffer_data + graphics->vertex_buffer_size, vertex_buffer_data, vertex_buffer_size);
    graphics->vertex_buffer_size += vertex_buffer_size;
}

uptr win32_graphics_create_input_layout(const u8* vertex_shader_buffer, u32 vertex_shader_buffer_size,
                                        const char** names, const u32* offsets, const u32* formats, u32 stride, u32 layout_count)
{
    Win32GraphicsInputLayout* input_layout = win32_memory_heap_allocate(sizeof(Win32GraphicsInputLayout), TRUE);
    u32 i = 0;

    for (i = 0; i < layout_count; ++i)
    {
        set_vertex_input_layout(input_layout, names[i], offsets[i], formats[i]);
    }

    // TODO: what to do with stride?
    input_layout->stride = stride;

    ID3D11Device_CreateInputLayout(graphics_device, input_layout->descs, input_layout->desc_count,
                                   vertex_shader_buffer, vertex_shader_buffer_size, &input_layout->layout);

    return (uptr)input_layout;
}

void win32_graphics_set_vertex_buffer_data(uptr graphics_pointer, const void* vertex_buffer_data, u32 vertex_buffer_size)
{
    Win32Graphics* graphics = (Win32Graphics*)graphics_pointer;

    memcpy(graphics->vertex_buffer_data, vertex_buffer_data, vertex_buffer_size);
    graphics->vertex_buffer_size = vertex_buffer_size;
}

void win32_graphics_add_vertex_buffer_data(uptr graphics_pointer, const void* vertex_buffer_data, u32 vertex_buffer_size)
{
    Win32Graphics* graphics = (Win32Graphics*)graphics_pointer;

    add_vertex_data(graphics, vertex_buffer_data, vertex_buffer_size);
}

void win32_graphics_clear(uptr graphics_pointer, RGBA color)
{
    Win32Graphics* graphics = (Win32Graphics*)graphics_pointer;

    graphics->clear_color[0] = (color.r + 0.5f) / 255.0f;
    graphics->clear_color[1] = (color.g + 0.5f) / 255.0f;
    graphics->clear_color[2] = (color.b + 0.5f) / 255.0f;
    graphics->clear_color[3] = (color.a + 0.5f) / 255.0f;
}

void win32_graphics_draw(uptr graphics_pointer)
{
    Win32Graphics* graphics = (Win32Graphics*)graphics_pointer;

    // NOTE: Resize the swap chain if size changed.
    resize_swap_chain(graphics);

    // NOTE: Draw everything.
    draw(graphics);
}

// TODO: We should be able to create (constant?) buffer.
// void win32_graphics_create_const_buffer(uptr graphics_pointer, u8* buffer, u32 buffer_size)
// {
//     Win32Graphics* graphics = (Win32Graphics*)graphics_pointer;
//     ID3D11Device* device = graphics->device;
//     ID3D11Buffer** const_buffer = &graphics->vertex_buffer;
//     D3D11_BUFFER_DESC desc =
//     {
//         .ByteWidth = buffer_size,
//         .Usage = D3D11_USAGE_DYNAMIC,
//         .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
//         .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
//     };

//     ID3D11Device_CreateBuffer(device, &desc, buffer, const_buffer);
// }

// TODO: We should think about D3D11_USAGE_IMMUTABLE but probably it
// is okay for textures.
uptr win32_graphics_create_texture_2D(uptr graphics_pointer, const u32* texture_buffer, i32 width, i32 height)
{
    Win32Graphics* graphics = (Win32Graphics*)graphics_pointer;
    ID3D11Device* device = graphics->device;
    // TODO: Maybe we need an internal arena to manage this kind of allocations
    // instead of allocating from heap everytime
    Win32GraphicsTexture2D* texture = win32_memory_heap_allocate(sizeof(Win32GraphicsTexture2D), TRUE);
    D3D11_TEXTURE2D_DESC desc =
    {
        .Width = (UINT)width,
        .Height = (UINT)height,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .SampleDesc = { 1, 0 },
        .Usage = D3D11_USAGE_IMMUTABLE,
        .BindFlags = D3D11_BIND_SHADER_RESOURCE,
    };
    D3D11_SUBRESOURCE_DATA data =
    {
        .pSysMem = texture_buffer,
        .SysMemPitch = (u32)width * sizeof(u32),
    };

    ID3D11Device_CreateTexture2D(device, &desc, &data, &texture->texture);
    ID3D11Device_CreateShaderResourceView(device, (ID3D11Resource*)texture->texture, NULL, &texture->view);
    // TODO: Is it okay to release texture source immediately after
    //  creating shader resource view or we will need it?
    // ID3D11Texture2D_Release(texture);

    return (uptr)texture;
}

void win32_graphics_use_texture_2Ds(uptr graphics_pointer, uptr* texture_2Ds, u32 texture_count)
{
    Win32Graphics* graphics = (Win32Graphics*)graphics_pointer;

    if (texture_2Ds == 0 || texture_count == 0)
    {
        graphics->texture_count = 0;
    }
    else
    {
        ASSERT(texture_count < ARRAY_COUNT(graphics->texture_views));

        for (u32 i = 0; i < texture_count; ++i)
        {
            graphics->texture_views[graphics->texture_count++] = ((Win32GraphicsTexture2D*)texture_2Ds[i])->view;
        }
    }
}

uptr win32_graphics_create_vertex_shader(const u8* shader_buffer, u32 shader_buffer_size)
{
    Win32GraphicsShader* shader = win32_memory_heap_allocate(sizeof(Win32GraphicsShader), TRUE);

    shader->type = WIN32_GRAPHICS_SHADER_TYPE_VERTEX;
    shader->vertex_shader.shader = 0; 

    // NOTE: Alternative to hlsl compilation at runtime is to precompile shaders offline
    // it improves startup time - no need to parse hlsl files at runtime!
    // and it allows to remove runtime dependency on d3dcompiler dll file
    // a) save shader source code into "shader.hlsl" file
    // b) run hlsl compiler to compile shader, these run compilation with optimizations and without debug info:
    // fxc.exe /nologo /T vs_5_0 /E vs /O3 /WX /Zpc /Ges /Fh ../source/win32/d3d11_vertex_shader.h /Vn d3d11_vertex_shader /Qstrip_reflect /Qstrip_debug /Qstrip_priv ../source/win32/shader.hlsl
    // fxc.exe /nologo /T ps_5_0 /E ps /O3 /WX /Zpc /Ges /Fh ../source/win32/d3d11_pixel_shader.h /Vn d3d11_pixel_shader /Qstrip_reflect /Qstrip_debug /Qstrip_priv ../source/win32/shader.hlsl
    // You can also use "/Fo d3d11_*shader.bin" argument to save compiled shader as binary file to store with your assets
    // then provide binary data for Create*Shader functions below without need to include shader bytes in C.

    ID3D11Device_CreateVertexShader(graphics_device, shader_buffer, shader_buffer_size,
                                    NULL, &shader->vertex_shader.shader);
    create_vertex_buffer(&shader->vertex_shader);

    return (uptr)shader;
}

uptr win32_graphics_create_pixel_shader(const u8* shader_buffer, u32 shader_buffer_size)
{
    Win32GraphicsShader* shader = win32_memory_heap_allocate(sizeof(Win32GraphicsShader), TRUE);
    
    shader->type = WIN32_GRAPHICS_SHADER_TYPE_PIXEL;
    shader->pixel_shader.shader = 0; 
    
    ID3D11Device_CreatePixelShader(graphics_device, shader_buffer, shader_buffer_size,
                                   NULL, &shader->pixel_shader.shader);

    return (uptr)shader;
}

void win32_graphics_use_shader(uptr graphics_pointer, uptr shader_pointer)
{
    Win32Graphics* graphics = (Win32Graphics*)graphics_pointer;
    Win32GraphicsShader* shader = (Win32GraphicsShader*)shader_pointer;

    ASSERT(shader->type < WIN32_GRAPHICS_SHADER_TYPE_COUNT);
    graphics->shaders[shader->type] = shader;
}

void win32_graphics_use_input_layout(uptr graphics_pointer, uptr input_layout_pointer)
{
    Win32Graphics* graphics = (Win32Graphics*)graphics_pointer;
    Win32GraphicsInputLayout* input_layout = (Win32GraphicsInputLayout*)input_layout_pointer;
    
    graphics->input_layout = input_layout;
}

uptr win32_graphics_create_buffer(OSGraphicsBufferDesc* buffer_desc)
{
    Win32GraphicsState* graphics_state = get_graphics_state();
    Win32GraphicsBuffer* graphics_buffer = 0;

    PUSH_OR_GET_FROM_FREE_LIST(graphics_state, graphics_state->arena, buffer, graphics_buffer);
    ASSERT(graphics_buffer);

    D3D11_BUFFER_DESC desc =
    {
        .ByteWidth = (UINT)buffer_desc->size,
        .Usage = buffer_desc->usage, // TODO: Check buffer_desc->usage does it match with D3D11 enum values?
        .BindFlags = (buffer_desc->type == OS_GRAPHICS_BUFFER_TYPE_VERTEX_BUFFER) ? D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_INDEX_BUFFER,
        .CPUAccessFlags = (buffer_desc->usage == OS_GRAPHICS_USAGE_TYPE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0,
    };

    D3D11_SUBRESOURCE_DATA* data_pointer = 0;
    D3D11_SUBRESOURCE_DATA data = { 0 };

    if (buffer_desc->usage == OS_GRAPHICS_USAGE_TYPE_IMMUTABLE)
    {
        data.pSysMem = buffer_desc->data;
        data_pointer = &data;
    }
    
    ID3D11Device_CreateBuffer(graphics_device, &desc, data_pointer, &graphics_buffer->d3d11.buffer);

    return (uptr)graphics_buffer;
}

uptr win32_graphics_create_texture(OSGraphicsTextureDesc* texture_desc)
{
    Win32GraphicsTexture* graphics_texture = 0;
    Win32GraphicsState* graphics_state = get_graphics_state();

    PUSH_OR_GET_FROM_FREE_LIST(graphics_state, graphics_state->arena, texture, graphics_texture);
    ASSERT(graphics_texture);

    // TODO: Right now we only support one pixel format.
    ASSERT(texture_desc->format == OS_GRAPHICS_PIXEL_FORMAT_R8G8B8A8);
    ASSERT(texture_desc->type == OS_GRAPHICS_TEXTURE_TYPE_2D);

    graphics_texture->texture.width = texture_desc->width;
    graphics_texture->texture.height = texture_desc->height;
    graphics_texture->texture.type = texture_desc->type;
    graphics_texture->texture.usage = texture_desc->usage;
    graphics_texture->texture.format = texture_desc->format;
    graphics_texture->texture.render_target = texture_desc->render_target;

    // TODO: Right now we only support one pixel format.    
    graphics_texture->d3d11.format = DXGI_FORMAT_R8G8B8A8_UNORM;

    D3D11_SUBRESOURCE_DATA data =
    {
        .pSysMem = texture_desc->data,
        // TODO: Right now this is fixed because we have only one pixel format.
        .SysMemPitch = (u32)texture_desc->width * sizeof(u32),
    };
    
    D3D11_TEXTURE2D_DESC desc =
    {
        .Width = (UINT)graphics_texture->texture.width,
        .Height = (UINT)graphics_texture->texture.height,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = graphics_texture->d3d11.format,
        .SampleDesc = { 1, 0 },
        .Usage = graphics_texture->texture.usage == OS_GRAPHICS_USAGE_TYPE_DYNAMIC ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE,
        .BindFlags = D3D11_BIND_SHADER_RESOURCE,
        .CPUAccessFlags = (graphics_texture->texture.usage == OS_GRAPHICS_USAGE_TYPE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0,
    };
    
    ID3D11Device_CreateTexture2D(graphics_device, &desc, &data, &graphics_texture->d3d11.texture_2D);

    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc =
    {
        .Format = graphics_texture->d3d11.format,
        .ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
        .Texture2D.MipLevels = 1,
    };
    
    ID3D11Device_CreateShaderResourceView(graphics_device,
                                          (ID3D11Resource*)graphics_texture->d3d11.texture_2D, &shader_resource_view_desc,
                                          &graphics_texture->d3d11.shader_resource_view);

    graphics_texture->d3d11.resource = (ID3D11Resource*)graphics_texture->d3d11.texture_2D;
    
    return (uptr)graphics_texture;
}

uptr win32_graphics_create_shader(OSGraphicsShaderDesc* shader_desc)
{
    _Win32GraphicsShader* graphics_shader = 0;
    Win32GraphicsState* graphics_state = get_graphics_state();

    PUSH_OR_GET_FROM_FREE_LIST(graphics_state, graphics_state->arena, shader, graphics_shader);
    ASSERT(graphics_shader);

    for (u32 stage_index = 0; stage_index < OS_GRAPHICS_MAX_SHADER_STAGE_COUNT; ++stage_index)
    {
        OSGraphicsShaderStageDesc* stage_desc = ((stage_index == OS_GRAPHICS_VERTEX_SHADER_STAGE) ?
                                                 &shader_desc->vertex_shader : &shader_desc->pixel_shader);
        GraphicsShaderStage* stage = graphics_shader->shader.stage + stage_index;

        ASSERT(stage->uniform_block_count == 0);        
        for (u32 uniform_block_index = 0; uniform_block_index < OS_GRAPHICS_MAX_SHADER_STAGE_UNIFORM_COUNT; ++uniform_block_index)
        {
            OSGraphicsShaderUniformBlockDesc* uniform_block_desc = stage_desc->uniform_blocks + uniform_block_index;

            if (uniform_block_desc->size == 0)
            {
                break;
            }

            stage->uniform_blocks[uniform_block_index].size = uniform_block_desc->size;
            ++stage->uniform_block_count;
        }

        ASSERT(stage->texture_count == 0);        
        for (u32 texture_index = 0; texture_index < OS_GRAPHICS_MAX_SHADER_STAGE_TEXTURE_COUNT; ++texture_index)
        {
            OSGraphicsShaderTextureDesc* texture_desc = stage_desc->textures + texture_index;

            if (texture_desc->used == 0)
            {
                break;
            }

            stage->textures[texture_index].texture_type = texture_desc->texture_type;
            ++stage->texture_count;
        }
    }
    
    // TODO: Maybe deep-copy?
    graphics_shader->d3d11.vertex_shader_data = shader_desc->vertex_shader.byte_code;
    graphics_shader->d3d11.vertex_shader_data_size = shader_desc->vertex_shader.byte_code_size;

    ID3D11Device_CreateVertexShader(graphics_device, shader_desc->vertex_shader.byte_code,
                                    shader_desc->vertex_shader.byte_code_size,
                                    NULL, &graphics_shader->d3d11.vertex_shader);

    ID3D11Device_CreatePixelShader(graphics_device, shader_desc->pixel_shader.byte_code,
                                    shader_desc->pixel_shader.byte_code_size,
                                    NULL, &graphics_shader->d3d11.pixel_shader);

    for (u32 attribute_index = 0; attribute_index < OS_GRAPHICS_MAX_VERTEX_ATTRIBUTE_COUNT; ++attribute_index)
    {
        OSGraphicsShaderAttributeDesc* attribute = shader_desc->attributes + attribute_index;
        
        if (attribute->semantic_name)
        {
            Win32GraphicsShaderAttribute* d3d11_attribute = graphics_shader->d3d11.attributes + attribute_index;
            memory_size semantic_name_length = string_length(attribute->semantic_name);
            
            ASSERT(semantic_name_length < ARRAY_COUNT(d3d11_attribute->semantic_name));
            memory_copy(d3d11_attribute->semantic_name, attribute->semantic_name, semantic_name_length);
            d3d11_attribute->semantic_index = attribute->semantic_index;
        }
    }

    return (uptr)graphics_shader;
}

uptr win32_graphics_init(uptr handle_pointer)
{
    if (!_graphics_state)
    {
        MemoryArena* arena = allocate_memory_arena(MEGABYTES(4));
        _graphics_state = PUSH_STRUCT(arena, Win32GraphicsState);
        _graphics_state->arena = arena;
    }

    Win32GraphicsState* graphics_state = get_graphics_state();

    ASSERT(graphics_state && graphics_state->arena);

    Win32Graphics* graphics = PUSH_STRUCT(graphics_state->arena, Win32Graphics);
    HWND handle = (HWND)handle_pointer;

    graphics->handle = handle;

    create_device_and_context(graphics);
    create_swap_chain(graphics);
    create_sampler_state(graphics);
    create_blend_state(graphics);
    create_rasterizer_state(graphics);
    create_depth_state(graphics);

    // TODO: ???
    graphics->vertex_buffer_data = win32_memory_heap_allocate(VERTEX_BUFFER_SIZE, TRUE);
    graphics->vertex_buffer_size = 0;

    return (uptr)graphics;
}


// uptr win32_graphics_init(uptr handle_pointer)
// {
//     // TODO: Probably we should allocate dynamically for every call.
//     static Win32Graphics graphics = { 0 };
//     ID3D11Device* device = graphics.device;
//     ID3D11DeviceContext* context = graphics.context;
//     ID3D11ShaderResourceView* texture_view = graphics.texture_view;
//     ID3D11SamplerState* sampler_state = graphics.sampler_state;
//     ID3D11BlendState* blend_state = graphics.blend_state;
//     ID3D11RasterizerState* rasterizer_state = graphics.rasterizer_state;
//     ID3D11DepthStencilState* depth_state = graphics.depth_state;
//     ID3D11RenderTargetView* render_target_view = graphics.render_target_view;
//     ID3D11DepthStencilView* depth_stencil_view = graphics.depth_stencil_view;
//     IDXGISwapChain1* swap_chain = graphics.swap_chain;
//     HWND handle = (HWND)handle_pointer;
//     HRESULT hresult = { 0 };

//     ID3D11Buffer* buffer = 0;

//     graphics.handle = handle;

//     // NOTE: Create D3D11 device & context.
//     {
//         D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
//         UINT flags = 0;
// #ifndef NDEBUG
//         // NOTE: This enables VERY USEFUL debug messages in debugger output.
//         flags |= D3D11_CREATE_DEVICE_DEBUG;
// #endif

//         hresult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, levels, ARRAY_COUNT(levels),
//                                     D3D11_SDK_VERSION, &device, NULL, &context);
//         // NOTE: Make sure device creation succeeeds before continuing
//         // for simple application you could retry device creation with
//         // D3D_DRIVER_TYPE_WARP driver type which enables software
//         // rendering (could be useful on broken drivers or remote
//         // desktop situations).
//         ASSERT(SUCCEEDED(hresult));
//     }

// #ifndef NDEBUG
//     // NOTE: For debug builds enable VERY USEFUL debug break on API errors.
//     {
//         ID3D11InfoQueue* info = 0;

//         ID3D11Device_QueryInterface(device, &IID_ID3D11InfoQueue, (void**)&info);
//         ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
//         ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
//         ID3D11InfoQueue_Release(info);
//     }
//     // NOTE: After this there is no need to check for any errors on
//     // device functions manually so all HRESULT return values in this
//     // code will be ignored debugger will break on errors anyway.
// #endif
//     {
//         // NOTE: Create DXGI swap chain.
//         {
//             // NOTE: Create DXGI 1.2 factory for creating swap chain.
//             IDXGIFactory2* factory = 0;

//             hresult = CreateDXGIFactory(&IID_IDXGIFactory2, (void**)&factory);
//             ASSERT(SUCCEEDED(hresult));

//             {
//                 DXGI_SWAP_CHAIN_DESC1 desc =
//                 {
//                     // default 0 value for width & height means to get it from HWND automatically
//                     //.Width = 0,
//                     //.Height = 0,

//                     // or use DXGI_FORMAT_R8G8B8A8_UNORM_SRGB for storing sRGB
//                     .Format = DXGI_FORMAT_R8G8B8A8_UNORM,

//                     // NOTE: FLIP presentation model does not allow
//                     // MSAA framebuffer if you want MSAA then you'll
//                     // need to render offscreen and manually resolve
//                     // to non-MSAA framebuffer.
//                     .SampleDesc = { 1, 0 },

//                     .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
//                     .BufferCount = 2,

//                     // NOTE: We don't want any automatic scaling of
//                     // window content this is supported only on FLIP
//                     // presentation model.
//                     .Scaling = DXGI_SCALING_NONE,

//                     // NOTE: Use more efficient FLIP presentation model
//                     // Windows 10 allows to use DXGI_SWAP_EFFECT_FLIP_DISCARD
//                     // for Windows 8 compatibility use DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
//                     // for Windows 7 compatibility use DXGI_SWAP_EFFECT_DISCARD.
//                     .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
//                 };

//                 hresult = IDXGIFactory2_CreateSwapChainForHwnd(factory, (IUnknown*)device, handle, &desc, NULL, NULL, &swap_chain);
//                 // NOTE: Make sure swap chain creation succeeds before continuing.
//                 ASSERT(SUCCEEDED(hresult));

//                 IDXGIFactory_Release(factory);
//             }
//         }

//         // NOTE: Disable Alt + Enter changing monitor resolution to match window size.
//         {
//             IDXGIFactory* factory = 0;

//             IDXGISwapChain1_GetParent(swap_chain, &IID_IDXGIFactory, (void**)&factory);
//             IDXGIFactory_MakeWindowAssociation(factory, handle, DXGI_MWA_NO_ALT_ENTER);
//             IDXGIFactory_Release(factory);
//         }
//     }

//     // TODO: This part should not be in init.
//     {
//         struct Vertex
//         {
//             float position[2];
//             float uv[2];
//             float color[3];
//         };

//         ID3D11Buffer* vertex_buffer = 0;
//         {
//             struct Vertex data[] =
//             {
//                 { { -0.00f, +0.75f }, { 25.0f, 50.0f }, { 1, 0, 0 } },
//                 { { +0.75f, -0.50f }, {  0.0f,  0.0f }, { 0, 1, 0 } },
//                 { { -0.75f, -0.50f }, { 50.0f,  0.0f }, { 0, 0, 1 } },
//             };

//             D3D11_BUFFER_DESC desc =
//             {
//                 .ByteWidth = sizeof(data),
//                 .Usage = D3D11_USAGE_IMMUTABLE,
//                 .BindFlags = D3D11_BIND_VERTEX_BUFFER,
//             };

//             D3D11_SUBRESOURCE_DATA initial_data = { .pSysMem = data };

//             ID3D11Device_CreateBuffer(device, &desc, &initial_data, &vertex_buffer);
//         }

//         {
//             // vertex & pixel shaders for drawing triangle, plus input layout for vertex input
//             ID3D11InputLayout* input_layout;
//             ID3D11VertexShader* vertex_shader;
//             ID3D11PixelShader* pixel_shader;
//             {
//                 // these must match vertex shader input layout (VS_INPUT in vertex shader source below)
//                 D3D11_INPUT_ELEMENT_DESC desc[] =
//                 {
//                     { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(struct Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
//                     { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(struct Vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
//                     { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(struct Vertex, color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
//                 };

// #if 0
//                 // alternative to hlsl compilation at runtime is to precompile shaders offline
//                 // it improves startup time - no need to parse hlsl files at runtime!
//                 // and it allows to remove runtime dependency on d3dcompiler dll file

//                 // a) save shader source code into "shader.hlsl" file
//                 // b) run hlsl compiler to compile shader, these run compilation with optimizations and without debug info:
//                 //      fxc.exe /nologo /T vs_5_0 /E vs /O3 /WX /Zpc /Ges /Fh d3d11_vshader.h /Vn d3d11_vshader /Qstrip_reflect /Qstrip_debug /Qstrip_priv shader.hlsl
//                 //      fxc.exe /nologo /T ps_5_0 /E ps /O3 /WX /Zpc /Ges /Fh d3d11_pshader.h /Vn d3d11_pshader /Qstrip_reflect /Qstrip_debug /Qstrip_priv shader.hlsl
//                 //    they will save output to d3d11_vshader.h and d3d11_pshader.h files
//                 // c) change #if 0 above to #if 1

//                 // you can also use "/Fo d3d11_*shader.bin" argument to save compiled shader as binary file to store with your assets
//                 // then provide binary data for Create*Shader functions below without need to include shader bytes in C

// #include "d3d11_vshader.h"
// #include "d3d11_pshader.h"

//                 ID3D11Device_CreateVertexShader(device, d3d11_vshader, sizeof(d3d11_vshader), NULL, &vertex_shader);
//                 ID3D11Device_CreatePixelShader(device, d3d11_pshader, sizeof(d3d11_pshader), NULL, &pixel_shader);
//                 ID3D11Device_CreateInputLayout(device, desc, ARRAYSIZE(desc), d3d11_vshader, sizeof(d3d11_vshader), &input_layout);
// #else
//                 const char hlsl[] =
//                 "#line " STRINGFY(__LINE__) "                                  \n\n" // actual line number in this file for nicer error messages
//                 "                                                           \n"
//                 "struct VS_INPUT                                            \n"
//                 "{                                                          \n"
//                 "     float2 pos   : POSITION;                              \n" // these names must match D3D11_INPUT_ELEMENT_DESC array
//                 "     float2 uv    : TEXCOORD;                              \n"
//                 "     float3 color : COLOR;                                 \n"
//                 "};                                                         \n"
//                 "                                                           \n"
//                 "struct PS_INPUT                                            \n"
//                 "{                                                          \n"
//                 "  float4 pos   : SV_POSITION;                              \n" // these names do not matter, except SV_... ones
//                 "  float2 uv    : TEXCOORD;                                 \n"
//                 "  float4 color : COLOR;                                    \n"
//                 "};                                                         \n"
//                 "                                                           \n"
//                 "cbuffer cbuffer0 : register(b0)                            \n" // b0 = constant buffer bound to slot 0
//                 "{                                                          \n"
//                 "    float4x4 uTransform;                                   \n"
//                 "}                                                          \n"
//                 "                                                           \n"
//                 "sampler sampler0 : register(s0);                           \n" // s0 = sampler bound to slot 0
//                 "                                                           \n"
//                 "Texture2D<float4> texture0 : register(t0);                 \n" // t0 = shader resource bound to slot 0
//                 "                                                           \n"
//                 "PS_INPUT vs(VS_INPUT input)                                \n"
//                 "{                                                          \n"
//                 "    PS_INPUT output;                                       \n"
//                 "    output.pos = mul(uTransform, float4(input.pos, 0, 1)); \n"
//                 "    output.uv = input.uv;                                  \n"
//                 "    output.color = float4(input.color, 1);                 \n"
//                 "    return output;                                         \n"
//                 "}                                                          \n"
//                 "                                                           \n"
//                 "float4 ps(PS_INPUT input) : SV_TARGET                      \n"
//                 "{                                                          \n"
//                 "    float4 tex = texture0.Sample(sampler0, input.uv);      \n"
//                 "    return input.color * tex;                              \n"
//                 "}                                                          \n";

//                 ID3DBlob* error = 0;
//                 ID3DBlob* vertex_blob = 0;
//                 ID3DBlob* pixel_blob = 0;

//                 UINT flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
// #ifndef NDEBUG
//                 flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
// #else
//                 flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
// #endif

//                 hresult = D3DCompile(hlsl, sizeof(hlsl), NULL, NULL, NULL, "vs", "vs_5_0", flags, 0, &vertex_blob, &error);
//                 if (FAILED(hresult))
//                 {
//                     const char* message = ID3D10Blob_GetBufferPointer(error);
//                     OutputDebugStringA(message);
//                     // ASSERT(!"Failed to compile vertex shader!");
//                 }

//                 hresult = D3DCompile(hlsl, sizeof(hlsl), NULL, NULL, NULL, "ps", "ps_5_0", flags, 0, &pixel_blob, &error);
//                 if (FAILED(hresult))
//                 {
//                     const char* message = ID3D10Blob_GetBufferPointer(error);
//                     OutputDebugStringA(message);
//                     // ASSERT(!"Failed to compile pixel shader!");
//                 }

//                 ID3D11Device_CreateVertexShader(device, ID3D10Blob_GetBufferPointer(vertex_blob), ID3D10Blob_GetBufferSize(vertex_blob), NULL, &vertex_shader);
//                 ID3D11Device_CreatePixelShader(device, ID3D10Blob_GetBufferPointer(pixel_blob), ID3D10Blob_GetBufferSize(pixel_blob), NULL, &pixel_shader);
//                 ID3D11Device_CreateInputLayout(device, desc, ARRAYSIZE(desc), ID3D10Blob_GetBufferPointer(vertex_blob), ID3D10Blob_GetBufferSize(vertex_blob), &input_layout);

//                 ID3D10Blob_Release(pixel_blob);
//                 ID3D10Blob_Release(vertex_blob);
// #endif
//             }
//         }
//     }

//     {
//         D3D11_BUFFER_DESC desc =
//         {
//             // NOTE: Space for 4x4 float matrix (cbuffer0 from pixel shader).
//             .ByteWidth = 4 * 4 * sizeof(float),
//             .Usage = D3D11_USAGE_DYNAMIC,
//             .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
//             .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
//         };
//         ID3D11Device_CreateBuffer(device, &desc, NULL, &buffer);
//     }

//     {
//         // NOTE: Checkerboard texture, with 50% transparency on black colors.
//         u32 pixels[] =
//         {
//             0x80000000, 0xffffffff,
//             0xffffffff, 0x80000000,
//         };

//         UINT width = 2;
//         UINT height = 2;

//         D3D11_TEXTURE2D_DESC desc =
//         {
//             .Width = width,
//             .Height = height,
//             .MipLevels = 1,
//             .ArraySize = 1,
//             .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
//             .SampleDesc = { 1, 0 },
//             .Usage = D3D11_USAGE_IMMUTABLE,
//             .BindFlags = D3D11_BIND_SHADER_RESOURCE,
//         };

//         D3D11_SUBRESOURCE_DATA data =
//         {
//             .pSysMem = pixels,
//             .SysMemPitch = width * sizeof(unsigned int),
//         };

//         ID3D11Texture2D* texture = 0;

//         ID3D11Device_CreateTexture2D(device, &desc, &data, &texture);
//         ID3D11Device_CreateShaderResourceView(device, (ID3D11Resource*)texture, NULL, &texture_view);
//         ID3D11Texture2D_Release(texture);
//     }

//     // NOTE: Sampler state.
//     {
//         D3D11_SAMPLER_DESC desc =
//         {
//             .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
//             .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
//             .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
//             .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
//         };

//         ID3D11Device_CreateSamplerState(device, &desc, &sampler_state);
//     }

//     // NOTE: Blend state.
//     {
//         // NOTE: Enable alpha blending.
//         D3D11_BLEND_DESC desc =
//         {
//             .RenderTarget[0] =
//             {
//                 .BlendEnable = TRUE,
//                 .SrcBlend = D3D11_BLEND_SRC_ALPHA,
//                 .DestBlend = D3D11_BLEND_INV_SRC_ALPHA,
//                 .BlendOp = D3D11_BLEND_OP_ADD,
//                 .SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA,
//                 .DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA,
//                 .BlendOpAlpha = D3D11_BLEND_OP_ADD,
//                 .RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL,
//             },
//         };
//         ID3D11Device_CreateBlendState(device, &desc, &blend_state);
//     }

//     // NOTE: Rasterizer state.
//     {
//         // NOTE: Disable culling.
//         D3D11_RASTERIZER_DESC desc =
//         {
//             .FillMode = D3D11_FILL_SOLID,
//             .CullMode = D3D11_CULL_NONE,
//         };
//         ID3D11Device_CreateRasterizerState(device, &desc, &rasterizer_state);
//     }

//     // NOTE: Depth state.
//     {
//         // NOTE: Disable depth & stencil test.
//         D3D11_DEPTH_STENCIL_DESC desc =
//         {
//             .DepthEnable = FALSE,
//             .DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL,
//             .DepthFunc = D3D11_COMPARISON_LESS,
//             .StencilEnable = FALSE,
//             .StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK,
//             .StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK,
//             // .FrontFace = ...
//             // .BackFace = ...
//         };
//         ID3D11Device_CreateDepthStencilState(device, &desc, &depth_state);
//     }

//     {
//         // show the window
//         ShowWindow(window, SW_SHOWDEFAULT);

//         LARGE_INTEGER freq, c1;
//         QueryPerformanceFrequency(&freq);
//         QueryPerformanceCounter(&c1);

//         float angle = 0;
//         DWORD currentWidth = 0;
//         DWORD currentHeight = 0;

//         for (;;)
//         {
//             // process all incoming Windows messages
//             MSG msg;
//             if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
//             {
//                 if (msg.message == WM_QUIT)
//                 {
//                     break;
//                 }
//                 TranslateMessage(&msg);
//                 DispatchMessageW(&msg);
//                 continue;
//             }

//             resize_swap_chain(&graphics);

//             // can render only if window size is non-zero - we must have backbuffer & RenderTarget view created
//             if (render_target_view)
//             {
//                 LARGE_INTEGER c2;
//                 QueryPerformanceCounter(&c2);
//                 float delta = (float)((double)(c2.QuadPart - c1.QuadPart) / freq.QuadPart);
//                 c1 = c2;

//                 // output viewport covering all client area of window
//                 D3D11_VIEWPORT viewport =
//                 {
//                     .TopLeftX = 0,
//                     .TopLeftY = 0,
//                     .Width = (FLOAT)width,
//                     .Height = (FLOAT)height,
//                     .MinDepth = 0,
//                     .MaxDepth = 1,
//                 };

//                 // clear screen
//                 FLOAT color[] = { 0.392f, 0.584f, 0.929f, 1.f };
//                 ID3D11DeviceContext_ClearRenderTargetView(context, render_target_view, color);
//                 ID3D11DeviceContext_ClearDepthStencilView(context, depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

//                 // setup 4x4c rotation matrix in uniform
//                 {
//                     angle += delta * 2.0f * (float)M_PI / 20.0f; // full rotation in 20 seconds
//                     angle = fmodf(angle, 2.0f * (float)M_PI);

//                     float aspect = (float)height / width;
//                     float matrix[16] =
//                     {
//                         cosf(angle) * aspect, -sinf(angle), 0, 0,
//                         sinf(angle) * aspect,  cosf(angle), 0, 0,
//                         0,            0, 0, 0,
//                         0,            0, 0, 1,
//                     };

//                     D3D11_MAPPED_SUBRESOURCE mapped;
//                     ID3D11DeviceContext_Map(context, (ID3D11Resource*)buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
//                     memcpy(mapped.pData, matrix, sizeof(matrix));
//                     ID3D11DeviceContext_Unmap(context, (ID3D11Resource*)buffer, 0);
//                 }

//                 // Input Assembler
//                 ID3D11DeviceContext_IASetInputLayout(context, layout);
//                 ID3D11DeviceContext_IASetPrimitiveTopology(context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//                 UINT stride = sizeof(struct Vertex);
//                 UINT offset = 0;
//                 ID3D11DeviceContext_IASetVertexBuffers(context, 0, 1, &vertex_buffer, &stride, &offset);

//                 // Vertex Shader
//                 ID3D11DeviceContext_VSSetConstantBuffers(context, 0, 1, &buffer);
//                 ID3D11DeviceContext_VSSetShader(context, vertex_shader, NULL, 0);

//                 // Rasterizer Stage
//                 ID3D11DeviceContext_RSSetViewports(context, 1, &viewport);
//                 ID3D11DeviceContext_RSSetState(context, rasterizer_state);

//                 // Pixel Shader
//                 ID3D11DeviceContext_PSSetSamplers(context, 0, 1, &sampler_state);
//                 ID3D11DeviceContext_PSSetShaderResources(context, 0, 1, &texture_view);
//                 ID3D11DeviceContext_PSSetShader(context, pixel_shader, NULL, 0);

//                 // Output Merger
//                 ID3D11DeviceContext_OMSetBlendState(context, blend_state, NULL, ~0U);
//                 ID3D11DeviceContext_OMSetDepthStencilState(context, depth_state, 0);
//                 ID3D11DeviceContext_OMSetRenderTargets(context, 1, &render_target_view, depth_stencil_view);

//                 // draw 3 vertices
//                 ID3D11DeviceContext_Draw(context, 3, 0);
//             }

//             // change to FALSE to disable vsync
//             BOOL vsync = TRUE;
//             hr = IDXGISwapChain1_Present(swap_chain, vsync ? 1 : 0, 0);
//             if (hr == DXGI_STATUS_OCCLUDED)
//             {
//                 // window is minimized, cannot vsync - instead sleep a bit
//                 if (vsync)
//                 {
//                     Sleep(10);
//                 }
//             }
//             else if (FAILED(hr))
//             {
//                 FatalError("Failed to present swap chain! Device lost?");
//             }
//         }
//     }

//     return (uptr)&graphics;
// }
