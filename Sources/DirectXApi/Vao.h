#pragma once
#include <D3D11.h>
#include <vector>
#include "Types.h"

namespace DirectX
{
struct Buffer
{
    uint32 bindLocation;
    ID3D11Buffer* ptr;
};

const uint8 BUFFER_SIZE = 10;

class Object
{
public:
    Object()
    {
        for (auto& buffer : buffers_)
        {
            buffer.bindLocation = 0;
            buffer.ptr          = nullptr;
        }
    }

    uint32 indiciesSize = 0;
    Buffer buffers_[BUFFER_SIZE];

    void Bind(ID3D11DeviceContext* d11Context) const
    {
        int i = 0;
        for (auto& buffer : buffers_)
        {
            if (i == (int)VertexBufferObjects::INDICES or buffer.ptr == nullptr)
            {
                continue;
            }


        }
        d11Context->IASetIndexBuffer(buffers_[(int)VertexBufferObjects::INDICES].ptr, DXGI_FORMAT_R16_UINT, 0);
    }
};
}  // namespace DirectX
