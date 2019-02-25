#pragma once
#include <D3D11Shader.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx10.h>
#include <d3dx11.h>
#include <string>

namespace DirectX
{
HRESULT CompileShaderFromFile(const std::string& filename, LPCSTR szEntryPoint, LPCSTR szShaderModel,
                              ID3DBlob** ppBlobOut);

template<typename T>
void ReleasePtr(T*& ptr)
{
    if (ptr)
    {
        ptr->Release();
        ptr = nullptr;
    }
}
}  // namespace DirectX
