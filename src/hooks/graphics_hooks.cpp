#include <src/pch.h>

#include <src/cheat.h>
#include <src/overlay/overlay.h>

namespace Cheat
{
    static HRESULT WINAPI Hooked_Present(IDXGISwapChain* swapChain, UINT a1, UINT a2);
    static HRESULT WINAPI Hooked_ResizeBuffers(IDXGISwapChain* swapChain, UINT a1, UINT a2, UINT a3, DXGI_FORMAT a4, UINT a5);
    static void CreateRenderTarget(IDXGISwapChain* swapChain);

    void HookGraphics()
    {
        Hax::LogDebug(G->Logger, L"Hooking graphics...");

        WNDCLASSEX dummyWindow =
        {
            .cbSize = sizeof(WNDCLASSEX),
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = ::DefWindowProcW,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = ::GetModuleHandleW(NULL),
            .hIcon = NULL,
            .hCursor = NULL,
            .hbrBackground = NULL,
            .lpszMenuName = nullptr,
            .lpszClassName = L"Dummy window",
            .hIconSm = NULL
        };
        ::RegisterClassEx(&dummyWindow);
        HWND hWnd = ::CreateWindow(dummyWindow.lpszClassName, L"Dummy window", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, nullptr, nullptr, dummyWindow.hInstance, nullptr);

        DXGI_SWAP_CHAIN_DESC swapChainDesc =
        {
            .BufferDesc = {.Format = DXGI_FORMAT_R8G8B8A8_UNORM},
            .SampleDesc = {.Count = 1},
            .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
            .BufferCount = 2,
            .OutputWindow = hWnd,
            .Windowed = TRUE
        };

        const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

        ID3D11Device* device = nullptr;
        ID3D11DeviceContext* context = nullptr;
        IDXGISwapChain* swapChain = nullptr;

        HRESULT result = ::D3D11CreateDeviceAndSwapChain
        (
            NULL, D3D_DRIVER_TYPE_NULL, NULL, 0, featureLevels, 2,
            D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, nullptr, &context
        );
        HAX_PANIC(result == S_OK, &G->Logger, L"D3D11CreateDeviceAndSwapChain %d", result);

        void** vtable = *(void***)(swapChain);
        {
            DWORD oldProtect;
            BOOL ok = ::VirtualProtect(&vtable[8], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
            HAX_PANIC(ok == TRUE, &G->Logger, L"Present hook not installed");
            G->PresentOrig = vtable[8];
            vtable[8] = Hooked_Present;
            VirtualProtect(&vtable[8], sizeof(void*), oldProtect, &oldProtect);
        }
        {
            DWORD oldProtect;
            BOOL ok = VirtualProtect(&vtable[13], sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
            HAX_PANIC(ok == TRUE, &G->Logger, L"ResizeBuffers hook not installed");
            G->ResizeBuffersOrig = vtable[13];
            vtable[13] = Hooked_ResizeBuffers;
            VirtualProtect(&vtable[13], sizeof(void*), oldProtect, &oldProtect);
        }

        swapChain->Release();
        context->Release();
        device->Release();

        ::DestroyWindow(hWnd);
        ::UnregisterClass(dummyWindow.lpszClassName, dummyWindow.hInstance);
    }

    static HRESULT WINAPI Hooked_Present(IDXGISwapChain* swapChain, UINT a1, UINT a2)
    {
        if (!Hax::Gui::Initialized())
        {
            swapChain->GetDevice(IID_PPV_ARGS(&G->Device));
            G->Device->GetImmediateContext(&G->DeviceContext);

            DXGI_SWAP_CHAIN_DESC sd;
            swapChain->GetDesc(&sd);
            G->GameWndHandle = sd.OutputWindow;

            Hax::Gui::Initialize((Hax::Handle)sd.OutputWindow, G->Device);
            PrepareMenu();
            ToggleMenuVisibility();
            G->UvmThread = &UVM::ThreadAttach();
        }

        if (G->RenderTarget == nullptr)
            CreateRenderTarget(swapChain);

        //!
        G->NoGrabMaxTime &= !G->IsClient;
        G->Unbreakable &= !G->IsClient;
        G->EasyGrab &= !G->IsClient;
        G->InfBattery &= !G->IsClient;

        try
        {
            ID3D11RenderTargetView* gameRTV = nullptr;
            ID3D11DepthStencilView* backupDSV = nullptr;
            G->DeviceContext->OMGetRenderTargets(1, &gameRTV, &backupDSV);
            G->DeviceContext->OMSetRenderTargets(1, &G->RenderTarget, nullptr);

            Hax::Gui::BeginFrame();
            RenderOverlay();
            Hax::Gui::EndFrame();

            G->DeviceContext->OMSetRenderTargets(1, &gameRTV, backupDSV);
        }
        catch (System::Exception& ex)
        {
            System::String message = ex.GetMessage();
            Hax::LogError(G->Logger, L"%d: %ls", __LINE__, message ? message.begin() : L"Exception without message");
        }

        return ((HRESULT(WINAPI*)(IDXGISwapChain*, UINT, UINT))G->PresentOrig)(swapChain, a1, a2);
    }

    static HRESULT WINAPI Hooked_ResizeBuffers(IDXGISwapChain* swapChain, UINT a1, UINT a2, UINT a3, DXGI_FORMAT a4, UINT a5)
    {
        if (G->RenderTarget != nullptr)
        {
            G->RenderTarget->Release();
            G->RenderTarget = nullptr;
        }

        return ((HRESULT(WINAPI*)(IDXGISwapChain*,UINT,UINT,UINT,DXGI_FORMAT,UINT))G->ResizeBuffersOrig)(swapChain, a1, a2, a3, a4, a5);
    }

    static void CreateRenderTarget(IDXGISwapChain* swapChain)
    {
        ID3D11Texture2D* backBuffer = nullptr;
        swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        if (backBuffer != nullptr && G->Device != nullptr)
        {
            D3D11_RENDER_TARGET_VIEW_DESC desc = {.Format = DXGI_FORMAT_R8G8B8A8_UNORM, .ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D};
            G->Device->CreateRenderTargetView(backBuffer, &desc, &G->RenderTarget);
            backBuffer->Release();
        }
    }
}