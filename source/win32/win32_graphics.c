// NOTE: Resource: https://gist.github.com/mmozeiko/5e727f845db182d468a34d524508ad5f

#define COBJMACROS
#define STR2(x) #x
#define STR(x) STR2(x)

#include <Windows.h>

#pragma warning(push, 0)
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>
#pragma warning(pop)

#include "types.h"
#include "utils.h"
#include "win32_window.h"
#include "win32_graphics.h"

typedef struct Win32Graphics
{
    ID3D11Device* device;
    ID3D11DeviceContext* context;
} Win32Graphics;

uptr win32_graphics_init(uptr handle_pointer)
{
    // TODO: Probably we should allocate dynamically for every call.
    static Win32Graphics graphics = { 0 };
    ID3D11Device* device = graphics.device;
    ID3D11DeviceContext* context = graphics.context;
    HWND handle = (HWND)handle_pointer;
    HRESULT hresult = { 0 };
    
    ID3D11Buffer* buffer = 0;
    ID3D11ShaderResourceView* texture_view = 0;
    ID3D11SamplerState* sampler = 0;
    ID3D11BlendState* blend_state = 0;
    ID3D11RasterizerState* rasterizer_state = 0;
    ID3D11DepthStencilState* depth_state = 0;

    // NOTE: Create D3D11 device & context.
    {
        D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
        UINT flags = 0;
#ifndef NDEBUG
        // NOTE: This enables VERY USEFUL debug messages in debugger output.
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        
        hresult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, levels, ARRAY_COUNT(levels),
                                    D3D11_SDK_VERSION, &device, NULL, &context);
        // NOTE: Make sure device creation succeeeds before continuing
        // for simple application you could retry device creation with
        // D3D_DRIVER_TYPE_WARP driver type which enables software
        // rendering (could be useful on broken drivers or remote
        // desktop situations).
        ASSERT(SUCCEEDED(hresult));
    }

#ifndef NDEBUG
    // NOTE: For debug builds enable VERY USEFUL debug break on API errors.
    {
        ID3D11InfoQueue* info = 0;
    
        ID3D11Device_QueryInterface(device, &IID_ID3D11InfoQueue, (void**)&info);
        ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        ID3D11InfoQueue_Release(info);
    }
    // NOTE: After this there is no need to check for any errors on
    // device functions manually so all HRESULT return values in this
    // code will be ignored debugger will break on errors anyway.
#endif
    {
        // NOTE: Create DXGI swap chain.
        IDXGISwapChain1* swap_chain = 0;
        {
            // NOTE: Create DXGI 1.2 factory for creating swap chain.
            IDXGIFactory2* factory = 0;

            hresult = CreateDXGIFactory(&IID_IDXGIFactory2, (void**)&factory);
            ASSERT(SUCCEEDED(hresult));

            {
                DXGI_SWAP_CHAIN_DESC1 desc =
                {
                    // default 0 value for width & height means to get it from HWND automatically
                    //.Width = 0,
                    //.Height = 0,

                    // or use DXGI_FORMAT_R8G8B8A8_UNORM_SRGB for storing sRGB
                    .Format = DXGI_FORMAT_R8G8B8A8_UNORM,

                    // NOTE: FLIP presentation model does not allow
                    // MSAA framebuffer if you want MSAA then you'll
                    // need to render offscreen and manually resolve
                    // to non-MSAA framebuffer.
                    .SampleDesc = { 1, 0 },

                    .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                    .BufferCount = 2,

                    // NOTE: We don't want any automatic scaling of
                    // window content this is supported only on FLIP
                    // presentation model.
                    .Scaling = DXGI_SCALING_NONE,

                    // NOTE: Use more efficient FLIP presentation model
                    // Windows 10 allows to use DXGI_SWAP_EFFECT_FLIP_DISCARD
                    // for Windows 8 compatibility use DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
                    // for Windows 7 compatibility use DXGI_SWAP_EFFECT_DISCARD.
                    .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
                };

                hresult = IDXGIFactory2_CreateSwapChainForHwnd(factory, (IUnknown*)device, handle, &desc, NULL, NULL, &swap_chain);
                // NOTE: Make sure swap chain creation succeeds before continuing.
                ASSERT(SUCCEEDED(hresult));

                IDXGIFactory_Release(factory);
            }
        }

        // NOTE: Disable Alt + Enter changing monitor resolution to match window size.
        {
            IDXGIFactory* factory = 0;
        
            IDXGISwapChain1_GetParent(swap_chain, &IID_IDXGIFactory, (void**)&factory);
            IDXGIFactory_MakeWindowAssociation(factory, handle, DXGI_MWA_NO_ALT_ENTER);
            IDXGIFactory_Release(factory);
        }
    }

    // TODO: This part should not be in init.
    {
        struct Vertex
        {
            float position[2];
            float uv[2];
            float color[3];
        };

        ID3D11Buffer* vertex_buffer = 0;
        {
            struct Vertex data[] =
            {
                { { -0.00f, +0.75f }, { 25.0f, 50.0f }, { 1, 0, 0 } },
                { { +0.75f, -0.50f }, {  0.0f,  0.0f }, { 0, 1, 0 } },
                { { -0.75f, -0.50f }, { 50.0f,  0.0f }, { 0, 0, 1 } },
            };

            D3D11_BUFFER_DESC desc =
            {
                .ByteWidth = sizeof(data),
                .Usage = D3D11_USAGE_IMMUTABLE,
                .BindFlags = D3D11_BIND_VERTEX_BUFFER,
            };

            D3D11_SUBRESOURCE_DATA initial_data = { .pSysMem = data };

            ID3D11Device_CreateBuffer(device, &desc, &initial_data, &vertex_buffer);
        }

        {
            // vertex & pixel shaders for drawing triangle, plus input layout for vertex input
            ID3D11InputLayout* input_layout;
            ID3D11VertexShader* vertex_shader;
            ID3D11PixelShader* pixel_shader;
            {
                // these must match vertex shader input layout (VS_INPUT in vertex shader source below)
                D3D11_INPUT_ELEMENT_DESC desc[] =
                {
                    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(struct Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
                    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(struct Vertex, uv),       D3D11_INPUT_PER_VERTEX_DATA, 0 },
                    { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(struct Vertex, color),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
                };

#if 0
                // alternative to hlsl compilation at runtime is to precompile shaders offline
                // it improves startup time - no need to parse hlsl files at runtime!
                // and it allows to remove runtime dependency on d3dcompiler dll file

                // a) save shader source code into "shader.hlsl" file
                // b) run hlsl compiler to compile shader, these run compilation with optimizations and without debug info:
                //      fxc.exe /nologo /T vs_5_0 /E vs /O3 /WX /Zpc /Ges /Fh d3d11_vshader.h /Vn d3d11_vshader /Qstrip_reflect /Qstrip_debug /Qstrip_priv shader.hlsl
                //      fxc.exe /nologo /T ps_5_0 /E ps /O3 /WX /Zpc /Ges /Fh d3d11_pshader.h /Vn d3d11_pshader /Qstrip_reflect /Qstrip_debug /Qstrip_priv shader.hlsl
                //    they will save output to d3d11_vshader.h and d3d11_pshader.h files
                // c) change #if 0 above to #if 1

                // you can also use "/Fo d3d11_*shader.bin" argument to save compiled shader as binary file to store with your assets
                // then provide binary data for Create*Shader functions below without need to include shader bytes in C

#include "d3d11_vshader.h"
#include "d3d11_pshader.h"

                ID3D11Device_CreateVertexShader(device, d3d11_vshader, sizeof(d3d11_vshader), NULL, &vertex_shader);
                ID3D11Device_CreatePixelShader(device, d3d11_pshader, sizeof(d3d11_pshader), NULL, &pixel_shader);
                ID3D11Device_CreateInputLayout(device, desc, ARRAYSIZE(desc), d3d11_vshader, sizeof(d3d11_vshader), &input_layout);
#else
                const char hlsl[] =
                "#line " STR(__LINE__) "                                  \n\n" // actual line number in this file for nicer error messages
                "                                                           \n"
                "struct VS_INPUT                                            \n"
                "{                                                          \n"
                "     float2 pos   : POSITION;                              \n" // these names must match D3D11_INPUT_ELEMENT_DESC array
                "     float2 uv    : TEXCOORD;                              \n"
                "     float3 color : COLOR;                                 \n"
                "};                                                         \n"
                "                                                           \n"
                "struct PS_INPUT                                            \n"
                "{                                                          \n"
                "  float4 pos   : SV_POSITION;                              \n" // these names do not matter, except SV_... ones
                "  float2 uv    : TEXCOORD;                                 \n"
                "  float4 color : COLOR;                                    \n"
                "};                                                         \n"
                "                                                           \n"
                "cbuffer cbuffer0 : register(b0)                            \n" // b0 = constant buffer bound to slot 0
                "{                                                          \n"
                "    float4x4 uTransform;                                   \n"
                "}                                                          \n"
                "                                                           \n"
                "sampler sampler0 : register(s0);                           \n" // s0 = sampler bound to slot 0
                "                                                           \n"
                "Texture2D<float4> texture0 : register(t0);                 \n" // t0 = shader resource bound to slot 0
                "                                                           \n"
                "PS_INPUT vs(VS_INPUT input)                                \n"
                "{                                                          \n"
                "    PS_INPUT output;                                       \n"
                "    output.pos = mul(uTransform, float4(input.pos, 0, 1)); \n"
                "    output.uv = input.uv;                                  \n"
                "    output.color = float4(input.color, 1);                 \n"
                "    return output;                                         \n"
                "}                                                          \n"
                "                                                           \n"
                "float4 ps(PS_INPUT input) : SV_TARGET                      \n"
                "{                                                          \n"
                "    float4 tex = texture0.Sample(sampler0, input.uv);      \n"
                "    return input.color * tex;                              \n"
                "}                                                          \n";

                ID3DBlob* error = 0;
                ID3DBlob* vertex_blob = 0;
                ID3DBlob* pixel_blob = 0;
            
                UINT flags = D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR | D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_WARNINGS_ARE_ERRORS;
#ifndef NDEBUG
                flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
                flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

                hresult = D3DCompile(hlsl, sizeof(hlsl), NULL, NULL, NULL, "vs", "vs_5_0", flags, 0, &vertex_blob, &error);
                if (FAILED(hresult))
                {
                    const char* message = ID3D10Blob_GetBufferPointer(error);
                    OutputDebugStringA(message);
                    // ASSERT(!"Failed to compile vertex shader!");
                }

                hresult = D3DCompile(hlsl, sizeof(hlsl), NULL, NULL, NULL, "ps", "ps_5_0", flags, 0, &pixel_blob, &error);
                if (FAILED(hresult))
                {
                    const char* message = ID3D10Blob_GetBufferPointer(error);
                    OutputDebugStringA(message);
                    // ASSERT(!"Failed to compile pixel shader!");
                }

                ID3D11Device_CreateVertexShader(device, ID3D10Blob_GetBufferPointer(vertex_blob), ID3D10Blob_GetBufferSize(vertex_blob), NULL, &vertex_shader);
                ID3D11Device_CreatePixelShader(device, ID3D10Blob_GetBufferPointer(pixel_blob), ID3D10Blob_GetBufferSize(pixel_blob), NULL, &pixel_shader);
                ID3D11Device_CreateInputLayout(device, desc, ARRAYSIZE(desc), ID3D10Blob_GetBufferPointer(vertex_blob), ID3D10Blob_GetBufferSize(vertex_blob), &input_layout);

                ID3D10Blob_Release(pixel_blob);
                ID3D10Blob_Release(vertex_blob);
#endif
            }
        }
    }

    {
        D3D11_BUFFER_DESC desc =
        {
            // NOTE: Space for 4x4 float matrix (cbuffer0 from pixel shader).
            .ByteWidth = 4 * 4 * sizeof(float),
            .Usage = D3D11_USAGE_DYNAMIC,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
        };
        ID3D11Device_CreateBuffer(device, &desc, NULL, &buffer);
    }

    {
        // NOTE: Checkerboard texture, with 50% transparency on black colors.
        u32 pixels[] =
        {
            0x80000000, 0xffffffff,
            0xffffffff, 0x80000000,
        };

        UINT width = 2;
        UINT height = 2;

        D3D11_TEXTURE2D_DESC desc =
        {
            .Width = width,
            .Height = height,
            .MipLevels = 1,
            .ArraySize = 1,
            .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
            .SampleDesc = { 1, 0 },
            .Usage = D3D11_USAGE_IMMUTABLE,
            .BindFlags = D3D11_BIND_SHADER_RESOURCE,
        };

        D3D11_SUBRESOURCE_DATA data =
        {
            .pSysMem = pixels,
            .SysMemPitch = width * sizeof(unsigned int),
        };

        ID3D11Texture2D* texture = 0;

        ID3D11Device_CreateTexture2D(device, &desc, &data, &texture);
        ID3D11Device_CreateShaderResourceView(device, (ID3D11Resource*)texture, NULL, &texture_view);
        ID3D11Texture2D_Release(texture);
    }

    // NOTE: Sampler.
    {
        D3D11_SAMPLER_DESC desc =
        {
            .Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
            .AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
            .AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
            .AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
        };

        ID3D11Device_CreateSamplerState(device, &desc, &sampler);
    }

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
        ID3D11Device_CreateBlendState(device, &desc, &blend_state);
    }

    // NOTE: Rasterizer state.
    {
        // NOTE: Disable culling.
        D3D11_RASTERIZER_DESC desc =
        {
            .FillMode = D3D11_FILL_SOLID,
            .CullMode = D3D11_CULL_NONE,
        };
        ID3D11Device_CreateRasterizerState(device, &desc, &rasterizer_state);
    }

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
        ID3D11Device_CreateDepthStencilState(device, &desc, &depth_state);
    }

    // ID3D11RenderTargetView* render_target_view = 0;
    // ID3D11DepthStencilView* depth_stencil_view = 0;

    return (uptr)&graphics;
}
