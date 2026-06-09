#pragma once
#include <d3dcompiler.h>
#include <string>

namespace GraphicsApi::Dx11
{
HRESULT CompileShaderFromFile(const std::string& filename, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

template <typename T>
void ReleasePtr(T*& ptr)
{
    if (ptr)
    {
        ptr->Release();
        ptr = nullptr;
    }
}
}  // namespace GraphicsApi::Dx11
