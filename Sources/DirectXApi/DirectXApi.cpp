#include "DirectXApi.h"

#include <DirectXMath.h>
#include <d3d11.h>
#include <d3d11shader.h>
#include <dxgi.h>
#include <windows.h>
#include <windowsx.h>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#undef CreateFont
#undef CreateWindow
#undef ANYSIZE
#include <Utils.h>
#include <Utils/Container.h>
#include <Utils/Variant.h>

#include "Buffer.h"
#include "DirectXContext.h"
#include "DirectXTools.h"
#include "DxFrameBuffer.h"
#include "DxShader.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GraphicsApi/MeshRawData.h"
#include "Logger/Log.h"
#include "Object.h"
#include "Shaders/DxForwardShaderFiles.h"
#include "Vao.h"
#include "Vertex.h"
#include "WinApi/WinApi.h"

namespace GraphicsApi::Dx11
{
namespace
{
TextureInfo defaultTextureInfo;
}
struct Quad : public Vao
{
    Quad()
    {
        vertexes_ = {
            {vec3(-1, 1, z), vec2(0, 0)},
            {vec3(-1, -1, z), vec2(0, 1)},
            {vec3(1, -1, z), vec2(1, 1)},
            {vec3(1, 1, z), vec2(1, 0)},
        };

        indices_ = {0, 1, 3, 3, 1, 2};
    }
    float z = 0;
};

struct Triangle : public Vao
{
    Triangle()
    {
        vertexes_ = {{vec3(0.0, 0.5, 0.5), vec2(0, 0)}, {vec3(0.5, -0.5, 0.5), vec2(0, 1)}, {vec3(-0.5, -0.5, 0.5), vec2(1, 1)}};
    }
};

struct Texture
{
    ID3D11ShaderResourceView *resourceView_ = nullptr;
    uint32 samplerStateId                   = 0;

    void Release()
    {
        ReleasePtr(resourceView_);
    }
};

HRESULT InitDevice(DirectXContext &directXContext)
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(directXContext.mainWindow, &rc);
    UINT width                   = rc.right - rc.left;
    UINT height                  = rc.bottom - rc.top;
    directXContext.viewPort.size = vec2ui(width, height);

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] = {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    const char *driverTypesStr[] = {
        "D3D_DRIVER_TYPE_HARDWARE",
        "D3D_DRIVER_TYPE_WARP",
        "D3D_DRIVER_TYPE_REFERENCE",
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount                        = 1;
    sd.BufferDesc.Width                   = width;
    sd.BufferDesc.Height                  = height;
    sd.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator   = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                       = directXContext.mainWindow;
    sd.SampleDesc.Count                   = 1;
    sd.SampleDesc.Quality                 = 0;
    sd.Windowed                           = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        directXContext.driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(NULL, directXContext.driverType, NULL, createDeviceFlags, featureLevels,
                                           numFeatureLevels, D3D11_SDK_VERSION, &sd, &directXContext.swapchain,
                                           &directXContext.dev, &directXContext.featureLevel, &directXContext.devcon);

        if (SUCCEEDED(hr))
        {
            LOG_DEBUG << "Use driver : " << driverTypesStr[driverTypeIndex];
            break;
        }
        else
        {
            LOG_ERROR << "Can not use driver : " << driverTypesStr[driverTypeIndex];
        }
    }
    if (FAILED(hr))
    {
        LOG_ERROR << "Init device error.";
        return hr;
    }

    return hr;
}

void InitViewPort(DirectXContext &directXContext)
{
    D3D11_VIEWPORT viewport;

    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width    = (FLOAT)directXContext.viewPort.size.x;
    viewport.Height   = (FLOAT)directXContext.viewPort.size.y;

    directXContext.devcon->RSSetViewports(1, &viewport);
}

template <typename T>
void Release(T &t)
{
    for (auto &v : t)
    {
        v.Release();
    }
}

struct GraphicsApiTextureInfo
{
    TextureFilter filter{TextureFilter::LINEAR};
    TextureMipmap mimap{TextureMipmap::LINEAR};
};

bool operator==(const GraphicsApiTextureInfo &a, const GraphicsApiTextureInfo &b)
{
    return a.filter == b.filter and a.mimap == b.mimap;
}

struct SamplerState
{
    ID3D11SamplerState *samplerState_{nullptr};
    GraphicsApiTextureInfo textureInfo_;

    void Release()
    {
        ReleasePtr(samplerState_);
    }
};

class DirectXApi::Pimpl
{
public:
    ID3D11BlendState *alphaBlendState = nullptr;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc_;
    std::vector<SamplerState> samplerStates_;
    uint32 quadId;
    DirectXContext dxContext_;
    std::vector<Buffer> buffers_;
    std::vector<DxShader> shaders_;
    std::vector<std::unique_ptr<IFrameBuffer>> frameBuffers_;
    ID3D11RasterizerState *rasterStateCullBack = nullptr;
    ID3D11RasterizerState *rasterStateCullNone = nullptr;

    const Object &GetDxObject(uint32 id)
    {
        return objects_[id - 1];
    }

    const Texture &GetTexture(uint32 id) const
    {
        return textures_[id - 1];
    }

    Texture &GetTexture(uint32 id)
    {
        return textures_[id - 1];
    }

    uint32 CreateAndAddDxObject(const Vao &vao)
    {
        objects_.emplace_back(dxContext_, vao);
        return objects_.size();
    }

    size_t CreateSamplerState(const GraphicsApiTextureInfo &textureInfo)
    {
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory(&sampDesc, sizeof(sampDesc));
        sampDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampDesc.MinLOD         = 0;
        sampDesc.MaxLOD         = D3D11_FLOAT32_MAX;
        sampDesc.MipLODBias     = 0;
        sampDesc.MaxAnisotropy  = 1;
        sampDesc.BorderColor[0] = sampDesc.BorderColor[1] = sampDesc.BorderColor[2] = sampDesc.BorderColor[3] = 0;

        ID3D11SamplerState *samplerState;
        auto hr = dxContext_.dev->CreateSamplerState(&sampDesc, &samplerState);

        if (FAILED(hr))
        {
            MessageBoxA(NULL, "Create texture error.", "Error", MB_OK);
        }
        samplerStates_.push_back({samplerState, textureInfo});
        return samplerStates_.size() - 1;
    }

    size_t GetSamplerState(const GraphicsApiTextureInfo &textureInfo)
    {
        size_t index = 0;
        for (auto &sampler : samplerStates_)
        {
            if (textureInfo == sampler.textureInfo_)
            {
                return index;
            }
            ++index;
        }
        return CreateSamplerState(textureInfo);
    }

    size_t AddTexture(ID3D11ShaderResourceView *rv, uint32 samplerStateId)
    {
        textures_.push_back({rv, samplerStateId});
        return textures_.size();
    }

    void SetPrimitivTopology(D3D_PRIMITIVE_TOPOLOGY topology)
    {
        if (lastUsePrimitiveTopology_ != topology)
        {
            dxContext_.devcon->IASetPrimitiveTopology(topology);
        }
    }
    const std::vector<Texture> &GetTextures() const
    {
        return textures_;
    }

    ~Pimpl()
    {
        ReleasePtr(alphaBlendState);
        Release(shaders_);
        Release(buffers_);
        Release(textures_);
        Release(objects_);
        dxContext_.Release();
    }

private:
    std::vector<Object> objects_;
    std::vector<Texture> textures_;
    D3D_PRIMITIVE_TOPOLOGY lastUsePrimitiveTopology_;
};

DirectXApi::DirectXApi()
{
    impl_      = std::make_unique<Pimpl>();
    windowApi_ = std::make_unique<WinApi>(impl_->dxContext_);

    bgColor_[0] = 0.f;
    bgColor_[1] = .2f;
    bgColor_[2] = .4f;
    bgColor_[3] = 1.f;
}
DirectXApi::~DirectXApi()
{
}

void DirectXApi::Init()
{
    if (FAILED(InitDevice(impl_->dxContext_)))
    {
        return;
    }

    SetRasterState();
    InitRenderTarget();
    InitViewPort(impl_->dxContext_);
    InitDepthSetncilView();
    SetRenderTargets();

    D3D11_BLEND_DESC dsc = {false,
                            false,
                            {true, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD, D3D11_BLEND_ZERO,
                             D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL}};

    auto hr = impl_->dxContext_.dev->CreateBlendState(&dsc, &impl_->alphaBlendState);

    if (FAILED(hr))
    {
        MessageBoxA(NULL, "CreateBlendState error.", __FUNCTION__, MB_OK);
        return;
    }

    impl_->SetPrimitivTopology(D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    impl_->quadId = impl_->CreateAndAddDxObject(Quad());

    // TO DO : default frame buffer
    impl_->frameBuffers_.push_back(std::make_unique<DxFrameBuffer>());
}
void DirectXApi::EndFrame()
{
    if (windowApi_)
        windowApi_->UpdateWindow();
}
IFrameBuffer &DirectXApi::GetDefaultFrameBuffer()
{
    if (impl_->frameBuffers_.empty())
    {
        LOG_ERROR << "Framebuffers are empty ! Default frame buffer should be at front!";
    }
    return *impl_->frameBuffers_.front();
}
IFrameBuffer &DirectXApi::CreateFrameBuffer(const std::vector<FrameBuffer::Attachment> &)
{
    impl_->frameBuffers_.push_back(std::make_unique<DxFrameBuffer>());
    return *impl_->frameBuffers_.back();
}
void DirectXApi::DeleteFrameBuffer(IFrameBuffer &frameBuffer)
{
    auto iter = std::find_if(impl_->frameBuffers_.begin(), impl_->frameBuffers_.end(),
                             [id = frameBuffer.GetId()](const auto &bufferPtr) { return bufferPtr->GetId() == id; });
    if (iter != impl_->frameBuffers_.end())
    {
        impl_->frameBuffers_.erase(iter);
    }
}
void DirectXApi::InitRenderTarget()
{
    // get the address of the back buffer
    ID3D11Texture2D *pBackBuffer = nullptr;
    impl_->dxContext_.swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);

    // use the back buffer address to create the render target
    impl_->dxContext_.dev->CreateRenderTargetView(pBackBuffer, NULL, &impl_->dxContext_.renderTargetView);
    pBackBuffer->Release();
}
void DirectXApi::InitDepthSetncilView()
{
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width              = impl_->dxContext_.viewPort.size.x;
    descDepth.Height             = impl_->dxContext_.viewPort.size.y;
    descDepth.MipLevels          = 1;
    descDepth.ArraySize          = 1;
    descDepth.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count   = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage              = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags     = 0;
    descDepth.MiscFlags          = 0;
    auto hr                      = impl_->dxContext_.dev->CreateTexture2D(&descDepth, NULL, &impl_->dxContext_.depthStencil);

    if (FAILED(hr))
    {
        MessageBoxA(NULL, "CreateTexture2D error.", __FUNCTION__, MB_OK);
        return;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format             = descDepth.Format;
    descDSV.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr                         = impl_->dxContext_.dev->CreateDepthStencilView(impl_->dxContext_.depthStencil, &descDSV,
                                                                               &impl_->dxContext_.depthStencilView);
    if (FAILED(hr))
    {
        MessageBoxA(NULL, "CreateDepthStencilView error.", __FUNCTION__, MB_OK);
    }
}

void DirectXApi::SetRasterState()
{
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode              = D3D11_CULL_BACK;
    rasterDesc.DepthBias             = 1;
    rasterDesc.DepthBiasClamp        = 0;
    rasterDesc.DepthClipEnable       = false;
    rasterDesc.FillMode              = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = true;
    rasterDesc.MultisampleEnable     = false;
    rasterDesc.ScissorEnable         = false;
    rasterDesc.SlopeScaledDepthBias  = 0.0f;

    auto result = impl_->dxContext_.dev->CreateRasterizerState(&rasterDesc, &impl_->dxContext_.rasterState);
    if (FAILED(result))
        return;
    impl_->dxContext_.devcon->RSSetState(impl_->dxContext_.rasterState);
}

void DirectXApi::SetRenderTargets()
{
    impl_->dxContext_.devcon->OMSetRenderTargets(1, &impl_->dxContext_.renderTargetView, impl_->dxContext_.depthStencilView);
}

void DirectXApi::GenerateMipmaps(IdType textureId)
{
    if (textureId >= impl_->GetTextures().size())
    {
        LOG_ERROR << "Texture ID out of range: " << textureId;
        return;
    }

    const auto &texture = impl_->GetTexture(textureId);

    if (texture.resourceView_)
    {
        impl_->dxContext_.devcon->GenerateMips(texture.resourceView_);

        LOG_DEBUG << "Mipmaps generated for DX11 texture ID: " << textureId;
    }
    else
    {
        LOG_ERROR << "Texture resourceView is null for ID: " << textureId;
    }
}

std::vector<Utils::Image> DirectXApi::GetImageArray(IdType) const
{
    return std::vector<Utils::Image>();
}

void DirectXApi::SetShadersFilesLocations(const std::filesystem::path &path)
{
    shadersFileLocation_ = path / "DirectXApi" / "HLSL";
}
void DirectXApi::SetShaderQuaility(ShaderQuaility)
{
}
void DirectXApi::DebugNormalMeshGeneration(bool)
{
}
void DirectXApi::CreateContext()
{
}
void DirectXApi::DeleteContext()
{
    impl_.release();
}
void DirectXApi::PrintVersion()
{
}
bool DirectXApi::IsTesselationSupported() const
{
    return false;
}
std::vector<RendererType> DirectXApi::GetSupportedRenderers() const
{
    return {RendererType::SIMPLE};
}
IWindowApi &DirectXApi::GetWindowApi()
{
    return *windowApi_;
}
void DirectXApi::PrepareFrame()
{
    impl_->dxContext_.devcon->ClearRenderTargetView(impl_->dxContext_.renderTargetView, bgColor_);
    impl_->dxContext_.devcon->ClearDepthStencilView(impl_->dxContext_.depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
                                                    1.0f, 0);
}
void DirectXApi::SetDefaultTarget()
{
}
void DirectXApi::SetBackgroundColor(const Color &background)
{
    bgColor_[0] = background.value.x;
    bgColor_[1] = background.value.y;
    bgColor_[2] = background.value.z;
    bgColor_[3] = background.value.w;
    bgColor     = background;
}

const Color &DirectXApi::GetBackgroundColor() const
{
    return bgColor;
}
void DirectXApi::EnableDepthTest()
{
    impl_->dxContext_.devcon->OMSetRenderTargets(1, &impl_->dxContext_.renderTargetView, impl_->dxContext_.depthStencilView);
}
void DirectXApi::DisableDepthTest()
{
    impl_->dxContext_.devcon->OMSetRenderTargets(1, &impl_->dxContext_.renderTargetView, nullptr);
}
ID DirectXApi::CreateShader(ShaderProgramType shaderType)
{
    if (not FileNameExist(shaderType))
    {
        return {};
    }

    auto filenames = GetDxForwardShaderFiles(shaderType);

    DxShader shader;

    auto vsShaderFileName = shadersFileLocation_ / filenames.at(ShaderType::VERTEX_SHADER);
    LOG_DEBUG << "Compiling " << vsShaderFileName << "...";
    auto hr = CompileShaderFromFile(vsShaderFileName.string(), "VS", "vs_4_0", &shader.blob_.vertex_);

    if (FAILED(hr))
    {
        auto failResult = "Vertex sheder can not be compiled. " + vsShaderFileName.string();
        MessageBoxA(NULL, failResult.c_str(), "Error", MB_OK);
        return {};
    }
    hr = impl_->dxContext_.dev->CreateVertexShader(shader.blob_.vertex_->GetBufferPointer(),
                                                   shader.blob_.vertex_->GetBufferSize(), NULL, &shader.vertex_);
    if (FAILED(hr))
    {
        auto failResult = vsShaderFileName.string() + " create vertex shader error";
        MessageBoxA(NULL, failResult.c_str(), __FUNCTION__, MB_OK);
        shader.blob_.Release();
        return {};
    }

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
    hr = impl_->dxContext_.dev->CreateInputLayout(layout, numElements, shader.blob_.vertex_->GetBufferPointer(),
                                                  shader.blob_.vertex_->GetBufferSize(), &shader.vertexLayout_);
    if (FAILED(hr))
    {
        MessageBoxA(NULL,
                    "Can not create input layout."
                    "FX file.",
                    "Error", MB_OK);
        return {};
    }

    impl_->dxContext_.devcon->IASetInputLayout(shader.vertexLayout_);

    auto fragmentShaderFile = shadersFileLocation_ / filenames.at(ShaderType::FRAGMENT_SHADER);
    LOG_DEBUG << "Compiling " << fragmentShaderFile << "...";
    hr = CompileShaderFromFile(fragmentShaderFile.string(), "PS", "ps_4_0", &shader.blob_.pixel_);

    if (FAILED(hr))
    {
        auto fail = "Fragment sheder can not be compiled. " + fragmentShaderFile.string();
        MessageBoxA(NULL, fail.c_str(), "Error", MB_OK);
        return {};
    }

    // Create the pixel shader
    hr = impl_->dxContext_.dev->CreatePixelShader(shader.blob_.pixel_->GetBufferPointer(), shader.blob_.pixel_->GetBufferSize(),
                                                  NULL, &shader.pixel_);
    shader.blob_.Release();

    if (FAILED(hr))
    {
        MessageBoxA(NULL, "Can pixel shader error.", __FUNCTION__, MB_OK);
        return {};
    }

    impl_->shaders_.push_back(shader);
    LOG_DEBUG << "Shader created : " << vsShaderFileName;
    return impl_->shaders_.size();
}
void DirectXApi::UseShader(uint32 id)
{
    if (id == 0)
    {
        return;
    }
    const auto &shader = impl_->shaders_[id - 1];

    impl_->dxContext_.devcon->VSSetShader(shader.vertex_, NULL, 0);
    impl_->dxContext_.devcon->PSSetShader(shader.pixel_, NULL, 0);
    impl_->dxContext_.devcon->IASetInputLayout(shader.vertexLayout_);
}

ID DirectXApi::CreateShaderBuffer(uint32 bindLocation, uint32 size, DrawFlag)
{
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage          = D3D11_USAGE_DEFAULT;
    bd.ByteWidth      = size;
    bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;

    ID3D11Buffer *buffer;
    auto result = impl_->dxContext_.dev->CreateBuffer(&bd, NULL, &buffer);

    if (FAILED(result))
    {
        MessageBoxA(NULL, "ID3D11Buffer create error.", "Error", MB_OK);
        return {};
    }

    impl_->buffers_.push_back({bindLocation, buffer});

    return impl_->buffers_.size() - 1;
}
ID DirectXApi::CreateShaderStorageBuffer(uint32 bindLocation, uint32 size, DrawFlag flag)
{
    return {};
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    // Rozmiar SSBO w DX11 musi być wielokrotnością rozmiaru elementu (stride).
    // Załóżmy, że operujemy na strukturach lub floatach (np. stride = 4 lub wielkość struktury).
    // Dla uproszczenia przyjmijmy, że traktujemy to jako surowy bufor bajtów lub musisz znać 'stride'.
    uint32 elementStride = 4;  // To powinno być przekazywane jako parametr, np. sizeof(MyStruct)

    bd.Usage               = (flag == DrawFlag::Dynamic) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
    bd.ByteWidth           = size;
    bd.BindFlags           = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;  // SRV + UAV
    bd.CPUAccessFlags      = (flag == DrawFlag::Dynamic) ? D3D11_CPU_ACCESS_WRITE : 0;
    bd.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    bd.StructureByteStride = elementStride;

    ID3D11Buffer *buffer = nullptr;
    HRESULT hr           = impl_->dxContext_.dev->CreateBuffer(&bd, nullptr, &buffer);

    if (FAILED(hr))
    {
        return {};
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format              = DXGI_FORMAT_UNKNOWN;  // Wymagane dla StructuredBuffer
    srvDesc.ViewDimension       = D3D11_SRV_DIMENSION_BUFFER;
    srvDesc.Buffer.FirstElement = 0;
    srvDesc.Buffer.NumElements  = size / elementStride;

    ID3D11ShaderResourceView *srv = nullptr;
    impl_->dxContext_.dev->CreateShaderResourceView(buffer, &srvDesc, &srv);

    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
    ZeroMemory(&uavDesc, sizeof(uavDesc));
    uavDesc.Format              = DXGI_FORMAT_UNKNOWN;
    uavDesc.ViewDimension       = D3D11_UAV_DIMENSION_BUFFER;
    uavDesc.Buffer.FirstElement = 0;
    uavDesc.Buffer.NumElements  = size / elementStride;

    ID3D11UnorderedAccessView *uav = nullptr;
    impl_->dxContext_.dev->CreateUnorderedAccessView(buffer, &uavDesc, &uav);

    auto id = impl_->buffers_.size();
    impl_->buffers_.push_back({bindLocation, buffer, srv, uav});

    return id;
}
void DirectXApi::UpdateShaderBuffer(uint32 id, void const *data)
{
    if (not Utils::ValidateRange(impl_->buffers_, id))
    {
        MessageBoxA(NULL, "ID3D11Buffer update error. Size exceeded", "Error", MB_OK);
        return;
    }
    const auto &buffer = impl_->buffers_[id];
    impl_->dxContext_.devcon->UpdateSubresource(buffer.ptr, 0, NULL, data, 0, 0);
}

void DirectXApi::UpdateShaderStorageBuffer(uint32 id, void const *data, uint32 size)
{
    return;

    if (!Utils::ValidateRange(impl_->buffers_, id))
    {
        MessageBoxA(NULL, "ID3D11Buffer update error. Invalid ID", "Error", MB_OK);
        return;
    }
    const auto &buffer = impl_->buffers_[id];

    D3D11_BUFFER_DESC desc;
    buffer.ptr->GetDesc(&desc);

    if (desc.Usage == D3D11_USAGE_DYNAMIC)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT hr = impl_->dxContext_.devcon->Map(buffer.ptr, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        if (SUCCEEDED(hr))
        {
            memcpy(mappedResource.pData, data, size);
            impl_->dxContext_.devcon->Unmap(buffer.ptr, 0);
        }
    }
    else
    {
        impl_->dxContext_.devcon->UpdateSubresource(buffer.ptr, 0, NULL, data, 0, 0);
    }
}

void DirectXApi::UnmapShaderStorageBuffer(uint32 id)
{
}
void *DirectXApi::MapShaderStorageBuffer(uint32 id, uint32 size, uint32 flags)
{
    return nullptr;
}

uint32 DirectXApi::BindShaderBuffer(uint32 id)
{
    if (not Utils::ValidateRange(impl_->buffers_, id))
    {
        MessageBoxA(NULL, "BindShaderBuffer ID3D11Buffer. Size exceeded", "Error", MB_OK);
        return 0;
    }
    const auto &buffer = impl_->buffers_[id];
    impl_->dxContext_.devcon->VSSetConstantBuffers(buffer.bindLocation, 1, &buffer.ptr);
    impl_->dxContext_.devcon->PSSetConstantBuffers(buffer.bindLocation, 1, &buffer.ptr);
    return 0;  // to do return last binded buffer
}

ID3D11ShaderResourceView *CreateTexture2DDesc(DirectXContext &context, const vec2ui &size, const void *data,
                                              const std::optional<size_t> &pitch)
{
    ID3D11ShaderResourceView *rv;
    ID3D11Texture2D *texture2d;
    // D3DX11CreateShaderResourceViewFromFile(impl_->dxContext_.dev, "seafloor.dds", NULL, NULL, &rv, NULL);

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width     = size.x;
    desc.Height    = size.y;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    // desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count   = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage              = D3D11_USAGE_DEFAULT;
    desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    desc.CPUAccessFlags     = 0;
    desc.MiscFlags          = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = data;
    if (pitch)
    {
        subResource.SysMemPitch = *pitch;
    }
    else
    {
        subResource.SysMemPitch = desc.Width * 4;
    }

    subResource.SysMemSlicePitch = 0;
    auto result                  = context.dev->CreateTexture2D(&desc, &subResource, &texture2d);

    if (FAILED(result))
    {
        LOG_ERROR << "Create CreateTexture2D failed.";
        return nullptr;
    }

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format                    = desc.Format;
    srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels       = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    result                            = context.dev->CreateShaderResourceView(texture2d, &srvDesc, &rv);
    texture2d->Release();
    if (FAILED(result))
    {
        LOG_ERROR << "Create shaderResourceView failed.";
        return nullptr;
    }
    context.devcon->GenerateMips(rv);
    return rv;
}

ID DirectXApi::CreateTexture(const Utils::Image &image, TextureFilter filter,
                                          TextureMipmap mipmap)
{
    TextureType type{TextureType::DEPTH_BUFFER_2D};
    auto channels = image.getChannelsCount();
    std::visit(
        visitor{
            [&](const std::vector<uint8> &data)
            {
                switch (channels)
                {
                    case 4:
                        type = TextureType::U8_RGBA;
                        break;
                    default:
                        LOG_ERROR << "Not implmented.";
                }
            },
            [&](const std::vector<float> &data)
            {
                switch (channels)
                {
                    case 1:
                        type = TextureType::FLOAT_TEXTURE_1D;
                        break;
                    case 2:
                        type = TextureType::FLOAT_TEXTURE_2D;
                        break;
                    case 3:
                        type = TextureType::FLOAT_TEXTURE_3D;
                        break;
                    case 4:
                        type = TextureType::FLOAT_TEXTURE_4D;
                        break;
                    default:
                        LOG_ERROR << "Not implmented.";
                }
            },
            [](std::monostate) { LOG_ERROR << "Image data not set!"; },
        },
        image.getImageData());

    if (type != TextureType::U8_RGBA)
    {
        LOG_ERROR << "Not implmented.";
        return {};
    }

    auto resourceview = CreateTexture2DDesc(impl_->dxContext_, image.size(), image.getRawDataPtr(), image.pitch);

    if (not resourceview)
        return {};

    auto samplerId = impl_->GetSamplerState({filter, mipmap});
    return impl_->AddTexture(resourceview, samplerId);
}

ID DirectXApi::CreateTexture(const std::vector<Utils::Image> &images, TextureFilter filter,
                                          TextureMipmap mipmap)
{
    if (images.empty())
        return {};

    const auto &firstImg = images[0];
    uint32 width         = firstImg.size().x;
    uint32 height        = firstImg.size().y;
    uint32 arraySize     = static_cast<uint32>(images.size());

    D3D11_TEXTURE2D_DESC texDesc;
    ZeroMemory(&texDesc, sizeof(texDesc));
    texDesc.Width              = width;
    texDesc.Height             = height;
    texDesc.MipLevels          = 1;
    texDesc.ArraySize          = arraySize;
    texDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count   = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage              = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags     = 0;
    texDesc.MiscFlags          = 0;

    std::vector<D3D11_SUBRESOURCE_DATA> initData(arraySize);
    for (uint32 i = 0; i < arraySize; ++i)
    {
        initData[i].pSysMem          = images[i].getRawDataPtr();
        initData[i].SysMemPitch      = width * 4;
        initData[i].SysMemSlicePitch = 0;
    }

    ID3D11Texture2D *texPtr = nullptr;
    HRESULT hr              = impl_->dxContext_.dev->CreateTexture2D(&texDesc, initData.data(), &texPtr);

    if (FAILED(hr))
    {
        LOG_ERROR << "Failed to create Texture2DArray";
        return {};
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format                         = texDesc.Format;
    srvDesc.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels       = texDesc.MipLevels;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize       = arraySize;

    ID3D11ShaderResourceView *srv = nullptr;
    hr                            = impl_->dxContext_.dev->CreateShaderResourceView(texPtr, &srvDesc, &srv);

    texPtr->Release();

    if (FAILED(hr))
        return {};

    auto samplerId = impl_->GetSamplerState({filter, mipmap});
    return impl_->AddTexture(srv, samplerId);
}

std::optional<uint32> DirectXApi::CreateTextureStorage(TextureType, TextureFilter, int32)
{
    return std::optional<uint32>();
}
ID DirectXApi::CreateCubMapTexture(const std::array<Utils::Image, 6> &images)
{
    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    // D3DObjects to create
    ID3D11Texture2D *cubeTexture                 = NULL;
    ID3D11ShaderResourceView *shaderResourceView = NULL;

    // Description of each face
    D3D11_TEXTURE2D_DESC texDesc;
    texDesc.Width              = images[0].width;
    texDesc.Height             = images[0].height;
    texDesc.MipLevels          = 1;
    texDesc.ArraySize          = 6;
    texDesc.Format             = format;
    texDesc.CPUAccessFlags     = 0;
    texDesc.SampleDesc.Count   = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage              = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags          = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags     = 0;
    texDesc.MiscFlags          = D3D11_RESOURCE_MISC_TEXTURECUBE;

    // The Shader Resource view description
    D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
    SMViewDesc.Format                      = texDesc.Format;
    SMViewDesc.ViewDimension               = D3D11_SRV_DIMENSION_TEXTURECUBE;
    SMViewDesc.TextureCube.MipLevels       = texDesc.MipLevels;
    SMViewDesc.TextureCube.MostDetailedMip = 0;

    // Array to fill which we will use to point D3D at our loaded CPU images.
    D3D11_SUBRESOURCE_DATA pData[6];
    for (int cubeMapFaceIndex = 0; cubeMapFaceIndex < 6; cubeMapFaceIndex++)
    {
        // Pointer to the pixel data
        pData[cubeMapFaceIndex].pSysMem = images[cubeMapFaceIndex].getRawDataPtr();
        // Line width in bytes
        pData[cubeMapFaceIndex].SysMemPitch = texDesc.Width * 4;
        // This is only used for 3d textures.
        pData[cubeMapFaceIndex].SysMemSlicePitch = 0;
    }

    // Create the Texture Resource
    HRESULT hr = impl_->dxContext_.dev->CreateTexture2D(&texDesc, &pData[0], &cubeTexture);
    if (hr != S_OK)
    {
        LOG_ERROR << "CreateTexture2D for cube map error.";
        return std::nullopt;
    }

    // If we have created the texture resource for the six faces
    // we create the Shader Resource View to use in our shaders.
    hr = impl_->dxContext_.dev->CreateShaderResourceView(cubeTexture, &SMViewDesc, &shaderResourceView);
    if (hr != S_OK)
    {
        LOG_ERROR << "CreateShaderResourceView for cube map error.";
        return std::nullopt;
    }

    TextureFilter filter{TextureFilter::LINEAR};
    TextureMipmap mimap{TextureMipmap::NONE};

    auto samplerId = impl_->GetSamplerState({filter, mimap});
    return impl_->AddTexture(shaderResourceView, samplerId);
}
void DirectXApi::CopyTexture(IdType src, IdType dst, uint32 width, uint32 height)
{
}
void DirectXApi::UpdateTexture(uint32, const vec2ui &, const Utils::Image &)
{
}
void DirectXApi::UpdateTexture(uint32 id, const Utils::Image &image)
{
    // TO DO: maybe is better way to update texture than delete and create new one

    auto &texture = impl_->GetTexture(id);
    texture.Release();
    texture.resourceView_ = CreateTexture2DDesc(impl_->dxContext_, image.size(), image.getRawDataPtr(), image.pitch);
}

// void DirectXApi::ClearBuffer(BufferType type)
//{
//    FLOAT color[] = {0.0f, 0.2f, 0.4f, 1.0f};
//    if (type == BufferType::COLOR)
//        impl_->dxContext_.devcon->ClearRenderTargetView(impl_->dxContext_.renderTargetView, color);
//
//    if (type == BufferType::DEPTH)
//        impl_->dxContext_.devcon->ClearDepthStencilView(impl_->dxContext_.depthStencilView, D3D11_CLEAR_DEPTH, 1.0f,
//        0);
//}
void DirectXApi::ClearTexture(uint32, const Color &)
{
}
void DirectXApi::EnableBlend()
{
    float bf[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    impl_->dxContext_.devcon->OMSetBlendState(impl_->alphaBlendState, bf, 0xffffffff);
}
void DirectXApi::DisableBlend()
{
}
void DirectXApi::EnableDepthMask()
{
    impl_->depthStencilDesc_.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
}
void DirectXApi::DisableDepthMask()
{
    impl_->depthStencilDesc_.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
}
void DirectXApi::ActiveTexture(uint32 id)
{
    if (id == 0)
        return;

    const auto &texture      = impl_->GetTexture(id);
    const auto &samplerState = impl_->samplerStates_[texture.samplerStateId];
    impl_->dxContext_.devcon->PSSetShaderResources(0, 1, &texture.resourceView_);
    impl_->dxContext_.devcon->PSSetSamplers(0, 1, &samplerState.samplerState_);
}
void DirectXApi::ActiveTexture(uint32 nr, uint32 id)
{
    if (id == 0)
        return;

    const auto &texture      = impl_->GetTexture(id);
    const auto &samplerState = impl_->samplerStates_[texture.samplerStateId];

    if (nr < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1)
        impl_->dxContext_.devcon->PSSetShaderResources(nr, 1, &texture.resourceView_);

    // if ( < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1)
    impl_->dxContext_.devcon->PSSetSamplers(0, 1, &samplerState.samplerState_);
}
void DirectXApi::DeleteObject(uint32)
{
}
void DirectXApi::DeleteObject(const std::vector<uint32> &)
{
}
void DirectXApi::DeleteShaderBuffer(uint32)
{
}
void DirectXApi::DeleteShaderBuffer(const std::vector<uint32> &)
{
}
std::string DirectXApi::GetBufferStatus()
{
    return std::string();
}
ID DirectXApi::CreatePatchMesh(const std::vector<float> &)
{
    return {};
}
ID DirectXApi::CreatePurePatchMeshInstanced(uint32, uint32)
{
    return {};
}
ID DirectXApi::CreateMesh(const MeshRawData &meshData, RenderType rendertype)
{
    static_assert(sizeof(Vertex) == 76, "Błąd: Struktura Vertex musi mieć dokładnie 76 bajtów dla DX11 Layout!");

    Vao vao;
    size_t vertexCount = meshData.positions_.size() / 3;
    vao.vertexes_.resize(vertexCount);

    for (size_t i = 0; i < vertexCount; ++i)
    {
        Vertex &v = vao.vertexes_[i];

        size_t idx2 = i * 2;
        size_t idx3 = i * 3;
        size_t idx4 = i * 4;

        v.position = vec3(meshData.positions_[idx3], meshData.positions_[idx3 + 1], meshData.positions_[idx3 + 2]);

        if (idx3 + 2 < meshData.normals_.size())
        {
            v.normal = vec3(meshData.normals_[idx3], meshData.normals_[idx3 + 1], meshData.normals_[idx3 + 2]);
        }
        else
        {
            v.normal = vec3(0.0f, 1.0f, 0.0f);
        }
        if (idx3 + 2 < meshData.tangents_.size())
        {
            v.tangent = vec3(meshData.tangents_[idx3], meshData.tangents_[idx3 + 1], meshData.tangents_[idx3 + 2]);
        }
        else
        {
            v.tangent = vec3(0.0f, 0.0f, 0.0f);
        }

        if (idx2 + 1 < meshData.textCoords_.size())
        {
            v.textCoord = vec2(meshData.textCoords_[idx2], meshData.textCoords_[idx2 + 1]);
        }
        else
        {
            v.textCoord = vec2(0.0f, 0.0f);
        }

        if (idx4 + 3 < meshData.joinIds_.size() && !meshData.bonesWeights_.empty())
        {
            v.bonesIds = vec4i(meshData.joinIds_[idx4], meshData.joinIds_[idx4 + 1], meshData.joinIds_[idx4 + 2],
                               meshData.joinIds_[idx4 + 3]);

            v.weights = vec4(meshData.bonesWeights_[idx4], meshData.bonesWeights_[idx4 + 1], meshData.bonesWeights_[idx4 + 2],
                             meshData.bonesWeights_[idx4 + 3]);
        }
        else
        {
            v.bonesIds = vec4i(0, 0, 0, 0);
            v.weights  = vec4(0.0f, 0.0f, 0.0f, 0.0f);
        }
    }

    vao.indices_.reserve(meshData.indices_.size());
    for (auto i : meshData.indices_)
    {
        vao.indices_.push_back(i);
    }

    return impl_->CreateAndAddDxObject(vao);
}
ID DirectXApi::CreateDynamicLineMesh()
{
    return ID();
}
ID DirectXApi::CreateParticle()
{
    return {};
}
ID DirectXApi::CreateAnimatedParticle()
{
    return {};
}
void DirectXApi::Compute(uint32, uint32, uint32)
{
}
void DirectXApi::RenderPurePatchedMeshInstances(uint32)
{
}
void DirectXApi::RenderMesh(uint32 id)
{
    if (id == 0)
        return;

    impl_->SetPrimitivTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    impl_->GetDxObject(id).Draw();
}
void DirectXApi::RenderProcedural(uint32 count)
{
    if (count > 0)
    {
        impl_->dxContext_.devcon->IASetInputLayout(nullptr);

        ID3D11Buffer *nullBuffer = nullptr;
        uint32 stride            = 0;
        uint32 offset            = 0;
        impl_->dxContext_.devcon->IASetVertexBuffers(0, 1, &nullBuffer, &stride, &offset);
        impl_->dxContext_.devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        impl_->dxContext_.devcon->Draw(count, 0);
    }
}
void DirectXApi::RenderDebugNormals(uint32)
{
}
void DirectXApi::RenderTriangleStripMesh(uint32 id)
{
    if (id == 0)
        return;

    impl_->SetPrimitivTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    impl_->GetDxObject(id).Draw();
}
void DirectXApi::RenderMeshInstanced(uint32 id, uint32 instances)
{
    if (id == 0)
        return;

    impl_->SetPrimitivTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    impl_->GetDxObject(id).DrawInstanced(instances);
}
void DirectXApi::RenderPoints(uint32 id)
{
    if (id == 0)
        return;

    impl_->SetPrimitivTopology(D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
    impl_->GetDxObject(id).Draw();
}
void DirectXApi::RenderQuad()
{
    impl_->GetDxObject(impl_->quadId).Draw();
}
void DirectXApi::RenderQuadTs()
{
    impl_->GetDxObject(impl_->quadId).Draw();
}
void DirectXApi::EnableClipingPlane(uint32)
{
}
void DirectXApi::DisableCliping(uint32)
{
}
void DirectXApi::EnableCulling()
{
    if (!impl_->rasterStateCullBack)
    {
        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.AntialiasedLineEnable = false;
        rasterDesc.CullMode              = D3D11_CULL_BACK;
        rasterDesc.DepthBias             = 1;
        rasterDesc.DepthBiasClamp        = 0;
        rasterDesc.DepthClipEnable       = false;
        rasterDesc.FillMode              = D3D11_FILL_SOLID;
        rasterDesc.FrontCounterClockwise = true;
        rasterDesc.MultisampleEnable     = false;
        rasterDesc.ScissorEnable         = false;
        rasterDesc.SlopeScaledDepthBias  = 0.0f;

        impl_->dxContext_.dev->CreateRasterizerState(&rasterDesc, &impl_->rasterStateCullBack);
    }
    impl_->dxContext_.devcon->RSSetState(impl_->rasterStateCullBack);
}
void DirectXApi::DisableCulling()
{
    if (!impl_->rasterStateCullNone)
    {
        D3D11_RASTERIZER_DESC rasterDesc = {};
        rasterDesc.AntialiasedLineEnable = false;
        rasterDesc.CullMode              = D3D11_CULL_NONE;
        rasterDesc.DepthBias             = 1;
        rasterDesc.DepthBiasClamp        = 0;
        rasterDesc.DepthClipEnable       = false;
        rasterDesc.FillMode              = D3D11_FILL_SOLID;
        rasterDesc.FrontCounterClockwise = true;
        rasterDesc.MultisampleEnable     = false;
        rasterDesc.ScissorEnable         = false;
        rasterDesc.SlopeScaledDepthBias  = 0.0f;

        impl_->dxContext_.dev->CreateRasterizerState(&rasterDesc, &impl_->rasterStateCullNone);
    }
    impl_->dxContext_.devcon->RSSetState(impl_->rasterStateCullNone);
}
void DirectXApi::SetViewPort(uint32, uint32, uint32, uint32)
{
}
void DirectXApi::BindTexture(uint32 id) const
{
    if (id == 0)
        return;

    const auto &texture      = impl_->GetTexture(id);
    const auto &samplerState = impl_->samplerStates_[texture.samplerStateId];
    impl_->dxContext_.devcon->PSSetShaderResources(0, 1, &texture.resourceView_);
    impl_->dxContext_.devcon->PSSetSamplers(0, 1, &samplerState.samplerState_);
}
void DirectXApi::BindImageTexture(uint32, TextureAccess)
{
}
ID DirectXApi::CreateShadowMap(uint32, uint32)
{
    return {};
}
void DirectXApi::PolygonModeRender()
{
}
void DirectXApi::LineModeRender()
{
}
void DirectXApi::SetBlendFunction(BlendFunctionType)
{
}
void DirectXApi::UpdateMatrixes(uint32, const std::vector<mat4> &)
{
    LOG_ERROR << "Not implmented.";
}
void DirectXApi::UpdateMesh(uint32, const MeshRawData &, const std::set<VertexBufferObjects> &)
{
    LOG_ERROR << "Not implmented.";
}
void DirectXApi::UpdateLineMesh(uint32, const LineMesh &)
{
    LOG_ERROR << "Not implmented.";
}
void DirectXApi::UpdateOffset(uint32, const std::vector<vec4> &)
{
    LOG_ERROR << "Not implmented.";
}
void DirectXApi::UpdateBlend(uint32, const std::vector<float> &)
{
}
void DirectXApi::LoadViewMatrix(const mat4 &)
{
}
void DirectXApi::LoadProjectionMatrix(const mat4 &)
{
}
void DirectXApi::DrawLine(const vec3 &color, const vec3 &from, const vec3 &to)
{
}
mat4 DirectXApi::PrepareMatrixToLoad(const mat4 &m)
{
    return m;  //    glm::transpose(m);
}
std::vector<uint8> DirectXApi::GetTextureData(uint32) const
{
    return std::vector<uint8>();
}
const TextureInfo &DirectXApi::GetTextureInfo(uint32) const
{
    return defaultTextureInfo;
}
void DirectXApi::TakeSnapshoot(const std::filesystem::path &) const
{
}
std::optional<Utils::Image> DirectXApi::GetImage(IdType) const
{
    return std::optional<Utils::Image>();
}
std::optional<uint64_t> DirectXApi::GetBindlessHandle(IdType)
{
    return {};
}
}  // namespace GraphicsApi::Dx11
