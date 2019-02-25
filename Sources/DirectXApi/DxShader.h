#pragma once
#include <D3D11.h>
#include "DirectXTools.h"

namespace DirectX
{
struct Blob
{
    ID3DBlob* pixel_  = nullptr;
    ID3DBlob* vertex_ = nullptr;

    void Release()
    {
        ReleasePtr(pixel_);
        ReleasePtr(vertex_);
    }
};
struct DxShader
{
    Blob blob_;
    ID3D11VertexShader* vertex_      = nullptr;
    ID3D11PixelShader* pixel_        = nullptr;
    ID3D11InputLayout* vertexLayout_ = nullptr;

    void Release()
    {
        ReleasePtr(pixel_);
        ReleasePtr(vertex_);
        ReleasePtr(vertexLayout_);
        blob_.Release();
    }
};
}  // namespace DirectX
