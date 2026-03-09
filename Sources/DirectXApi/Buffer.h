#pragma once
#include <D3D11.h>
#include "Types.h"
#include "DirectXTools.h"

namespace DirectX
{
struct Buffer
{
    uint32 bindLocation;
    ID3D11Buffer* ptr = nullptr;
    ID3D11ShaderResourceView* srv = nullptr;
    ID3D11UnorderedAccessView* uav = nullptr;

    void Release()
    {
        ReleasePtr(ptr);
        ReleasePtr(srv);
        ReleasePtr(uav);
    }
};
}  // namespace DirectX
