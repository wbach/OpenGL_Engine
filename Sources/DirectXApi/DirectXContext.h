#pragma once
#include <D3D11.h>
#include "ViewPort.h"

namespace DirectX
{
struct DirectXContext
{
    ViewPort viewPort;
    IDXGISwapChain *swapchain                = nullptr;
    ID3D11Device *dev                        = nullptr;
    ID3D11DeviceContext *devcon              = nullptr;
    ID3D11RenderTargetView *backbuffer       = nullptr;
    ID3D11Texture2D *depthStencil            = nullptr;
    ID3D11DepthStencilView *depthStencilView = nullptr;

    ~DirectXContext() 
    {
        if (swapchain) swapchain->Release();
        if (depthStencil) depthStencil->Release();
        if (depthStencilView) depthStencilView->Release();
        if (backbuffer) backbuffer->Release();
        if (dev) dev->Release();
        if (devcon) devcon->Release();
    }
};
}  // namespace DirectX
