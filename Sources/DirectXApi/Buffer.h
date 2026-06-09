#pragma once
#include <d3d11.h>

#include "DirectXTools.h"
#include "Types.h"


namespace GraphicsApi::Dx11
{
struct Buffer
{
    uint32 bindLocation;
    ID3D11Buffer* ptr              = nullptr;
    ID3D11ShaderResourceView* srv  = nullptr;
    ID3D11UnorderedAccessView* uav = nullptr;

    void Release()
    {
        ReleasePtr(ptr);
        ReleasePtr(srv);
        ReleasePtr(uav);
    }
};
}  // namespace GraphicsApi::Dx11
