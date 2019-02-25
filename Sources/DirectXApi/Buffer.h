#pragma once
#include "Types.h"

class ID3D11Buffer;

namespace DirectX
{
struct Buffer
{
    uint32 bindLocation;
    ID3D11Buffer* ptr;
};
} // namespace DirectX
