#pragma once
#include <D3D11.h>

namespace DirectX
{
struct Blob
{
    ID3DBlob* pixel_  = nullptr;
    ID3DBlob* vertex_ = nullptr;

    //~Blob()
    //{
    //    if (pixel_ != nullptr)
    //    {
    //        pixel_->Release();
    //    }
    //    if (vertex_ != nullptr)
    //    {
    //        vertex_->Release();
    //    }
    //}
};
struct DxShader
{
    Blob blob_;
    ID3D11VertexShader* vertex_ = nullptr;
    ID3D11PixelShader* pixel_   = nullptr;
    ID3D11InputLayout* vertexLayout_ = nullptr;
    //~DxShader()
    //{
    //    if (vertex_ != nullptr)
    //    {
    //        vertex_->Release();
    //    }
    //    if (pixel_ != nullptr)
    //    {
    //        pixel_->Release();
    //    }
    //}
};
}  // namespace DirectX
