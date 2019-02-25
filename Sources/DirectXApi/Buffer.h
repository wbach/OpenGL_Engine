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

    void Release()
    {
        ReleasePtr(ptr);
    }
};
}  // namespace DirectX
