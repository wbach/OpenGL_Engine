#include "DirectXTools.h"

#include <Logger/Log.h>

namespace GraphicsApi::Dx11
{
HRESULT CompileShaderFromFile(const std::string& filename, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
#if defined(DEBUG) or defined(_DEBUG)
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    std::wstring wideFilename(filename.begin(), filename.end());

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(wideFilename.c_str(), nullptr, nullptr, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut,
                            &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
        {
            LOG_ERROR << static_cast<const char*>(pErrorBlob->GetBufferPointer());
            pErrorBlob->Release();
        }
        return hr;
    }

    if (pErrorBlob != nullptr)
    {
        pErrorBlob->Release();
    }

    return S_OK;
}
}  // namespace GraphicsApi::Dx11
