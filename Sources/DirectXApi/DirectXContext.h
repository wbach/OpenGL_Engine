#pragma once
#include <D3D11.h>
#include "DirectXTools.h"
#include "ViewPort.h"

namespace DirectX
{
struct DirectXContext
{
    HWND mainWindow;
    D3D_DRIVER_TYPE driverType     = D3D_DRIVER_TYPE_NULL;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    ViewPort viewPort;
    IDXGISwapChain *swapchain                  = nullptr;
    ID3D11Device *dev                          = nullptr;
    ID3D11DeviceContext *devcon                = nullptr;
    ID3D11RenderTargetView *renderTargetView   = nullptr;
    ID3D11Texture2D *depthStencil              = nullptr;
    ID3D11DepthStencilView *depthStencilView   = nullptr;
    ID3D11DepthStencilState *depthStencilState = nullptr;
    ID3D11RasterizerState *rasterState         = nullptr;

    void Release()
    {
        ReleasePtr(swapchain);
        ReleasePtr(dev);
        ReleasePtr(devcon);
        ReleasePtr(renderTargetView);
        ReleasePtr(depthStencilView);
        ReleasePtr(depthStencilState);
        ReleasePtr(rasterState);
    }
};
}  // namespace DirectX
