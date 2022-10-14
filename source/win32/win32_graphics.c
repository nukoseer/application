// NOTE: Resource: https://gist.github.com/mmozeiko/5e727f845db182d468a34d524508ad5f

#define COBJMACROS

#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d3dcompiler.h>

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

    // NOTE: Create D3D11 device & context.
    {
        D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
        UINT flags = 0;
#ifndef NDEBUG
        // this enables VERY USEFUL debug messages in debugger output
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        
        hresult = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, levels, ARRAY_COUNT(levels),
                                    D3D11_SDK_VERSION, &device, NULL, &context);
        // make sure device creation succeeeds before continuing
        // for simple applciation you could retry device creation with
        // D3D_DRIVER_TYPE_WARP driver type which enables software rendering
        // (could be useful on broken drivers or remote desktop situations)
        ASSERT(SUCCEEDED(hresult));
    }

#ifndef NDEBUG
    // for debug builds enable VERY USEFUL debug break on API errors
    {
        ID3D11InfoQueue* info = 0;
    
        ID3D11Device_QueryInterface(device, &IID_ID3D11InfoQueue, (void**)&info);
        ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        ID3D11InfoQueue_SetBreakOnSeverity(info, D3D11_MESSAGE_SEVERITY_ERROR, TRUE);
        ID3D11InfoQueue_Release(info);
    }
    // after this there's no need to check for any errors on device functions manually
    // so all HRESULT return  values in this code will be ignored
    // debugger will break on errors anyway
#endif
    {
        // NOTE: Create DXGI swap chain.
        IDXGISwapChain1* swap_chain = 0;
        {
            // create DXGI 1.2 factory for creating swap chain
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

                    // FLIP presentation model does not allow MSAA framebuffer
                    // if you want MSAA then you'll need to render offscreen and manually
                    // resolve to non-MSAA framebuffer
                    .SampleDesc = { 1, 0 },

                    .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                    .BufferCount = 2,

                    // we don't want any automatic scaling of window content
                    // this is supported only on FLIP presentation model
                    .Scaling = DXGI_SCALING_NONE,

                    // use more efficient FLIP presentation model
                    // Windows 10 allows to use DXGI_SWAP_EFFECT_FLIP_DISCARD
                    // for Windows 8 compatibility use DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL
                    // for Windows 7 compatibility use DXGI_SWAP_EFFECT_DISCARD
                    .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
                };

                hresult = IDXGIFactory2_CreateSwapChainForHwnd(factory, (IUnknown*)device, handle, &desc, NULL, NULL, &swap_chain);
                // make sure swap chain creation succeeds before continuing
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

    return (uptr)&graphics;
}
