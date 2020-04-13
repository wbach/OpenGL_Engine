#include <D3D11Shader.h>
#include <d3d11.h>
#include <d3dx10.h>
#include <d3dx11.h>
#include <windows.h>
#include <windowsx.h>
#include <xnamath.h>
#undef CreateFont
#undef CreateWindow
#include <string>

#include "Buffer.h"
#include "DirectXApi.h"
#include "DirectXContext.h"
#include "DirectXTools.h"
#include "DxShader.h"
#include "GraphicsApi/MeshRawData.h"
#include "Logger/Log.h"
#include "Object.h"
#include "Shaders/DxForwardShaderFiles.h"
#include "Utils.h"
#include "Vao.h"
#include "Vertex.h"
#include "WinApi/WinApi.h"

namespace DirectX
{
namespace
{
GraphicsApi::TextureInfo defaultTextureInfo;
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
        vertexes_ = {
            {vec3(0.0, 0.5, 0.5), vec2(0, 0)}, {vec3(0.5, -0.5, 0.5), vec2(0, 1)}, {vec3(-0.5, -0.5, 0.5), vec2(1, 1)}};
    }
};

struct Texture
{
    ID3D11ShaderResourceView *resourceView_ = nullptr;
    ID3D11SamplerState *samplerState_       = nullptr;

    void Release()
    {
        ReleasePtr(resourceView_);
        ReleasePtr(samplerState_);
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

    std::string driverTypesStr[] = {
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
            DEBUG_LOG("Use driver : " + driverTypesStr[driverTypeIndex]);
            break;
        }
        else
        {
            ERROR_LOG("Can not use driver : " + driverTypesStr[driverTypeIndex]);
        }
    }
    if (FAILED(hr))
    {
        ERROR_LOG("Init device error.");
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

class DirectXApi::Pimpl
{
public:
    ID3D11BlendState *alphaBlendState = nullptr;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc_;
    uint32 quadId;
    DirectXContext dxCondext_;
    std::vector<Buffer> buffers_;
    std::vector<DxShader> shaders_;

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
        objects_.emplace_back(dxCondext_, vao);
        return objects_.size();
    }

    Texture CreateDirectXTexture(const D3D11_SAMPLER_DESC &samplerDesc, ID3D11ShaderResourceView *rv)
    {
        Texture texture;
        texture.resourceView_ = rv;
        auto hr               = dxCondext_.dev->CreateSamplerState(&samplerDesc, &texture.samplerState_);

        if (FAILED(hr))
        {
            MessageBox(NULL, "Create texture error.", "Error", MB_OK);
        }
        return texture;
    }

    uint32 CreateTexture(const D3D11_SAMPLER_DESC &samplerDesc, ID3D11ShaderResourceView *rv)
    {
        textures_.push_back(CreateDirectXTexture(samplerDesc, rv));
        return textures_.size();
    }

    void SetPrimitivTopology(D3D_PRIMITIVE_TOPOLOGY topology)
    {
        if (lastUsePrimitiveTopology_ != topology)
        {
            dxCondext_.devcon->IASetPrimitiveTopology(topology);
        }
    }

    ~Pimpl()
    {
        ReleasePtr(alphaBlendState);
        Release(shaders_);
        Release(buffers_);
        Release(textures_);
        Release(objects_);
        dxCondext_.Release();
    }

private:
    std::vector<Object> objects_;
    std::vector<Texture> textures_;
    D3D_PRIMITIVE_TOPOLOGY lastUsePrimitiveTopology_;
};

DirectXApi::DirectXApi()
{
    impl_      = std::make_unique<Pimpl>();
    windowApi_ = std::make_unique<WinApi>(impl_->dxCondext_);

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
    if (FAILED(InitDevice(impl_->dxCondext_)))
    {
        return;
    }

    SetRasterState();
    InitRenderTarget();
    InitViewPort(impl_->dxCondext_);
    InitDepthSetncilView();
    SetRenderTargets();

    D3D11_BLEND_DESC dsc = {false,
                            false,
                            {true, D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
                             D3D11_BLEND_ZERO, D3D11_BLEND_ZERO, D3D11_BLEND_OP_ADD, D3D11_COLOR_WRITE_ENABLE_ALL}};

    auto hr = impl_->dxCondext_.dev->CreateBlendState(&dsc, &impl_->alphaBlendState);

    if (FAILED(hr))
    {
        MessageBox(NULL, "CreateBlendState error.", __FUNCTION__, MB_OK);
        return;
    }

    impl_->SetPrimitivTopology(D3D_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    impl_->quadId = impl_->CreateAndAddDxObject(Quad());
}
void DirectXApi::InitRenderTarget()
{
    // get the address of the back buffer
    ID3D11Texture2D *pBackBuffer = nullptr;
    impl_->dxCondext_.swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);

    // use the back buffer address to create the render target
    impl_->dxCondext_.dev->CreateRenderTargetView(pBackBuffer, NULL, &impl_->dxCondext_.renderTargetView);
    pBackBuffer->Release();
}
void DirectXApi::InitDepthSetncilView()
{
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width              = impl_->dxCondext_.viewPort.size.x;
    descDepth.Height             = impl_->dxCondext_.viewPort.size.y;
    descDepth.MipLevels          = 1;
    descDepth.ArraySize          = 1;
    descDepth.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count   = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage              = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags     = 0;
    descDepth.MiscFlags          = 0;
    auto hr = impl_->dxCondext_.dev->CreateTexture2D(&descDepth, NULL, &impl_->dxCondext_.depthStencil);

    if (FAILED(hr))
    {
        MessageBox(NULL, "CreateTexture2D error.", __FUNCTION__, MB_OK);
        return;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format             = descDepth.Format;
    descDSV.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr                         = impl_->dxCondext_.dev->CreateDepthStencilView(impl_->dxCondext_.depthStencil, &descDSV,
                                                       &impl_->dxCondext_.depthStencilView);
    if (FAILED(hr))
    {
        MessageBox(NULL, "CreateDepthStencilView error.", __FUNCTION__, MB_OK);
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

    auto result = impl_->dxCondext_.dev->CreateRasterizerState(&rasterDesc, &impl_->dxCondext_.rasterState);
    if (FAILED(result))
        return;
    impl_->dxCondext_.devcon->RSSetState(impl_->dxCondext_.rasterState);
}

void DirectXApi::SetRenderTargets()
{
    impl_->dxCondext_.devcon->OMSetRenderTargets(1, &impl_->dxCondext_.renderTargetView,
                                                 impl_->dxCondext_.depthStencilView);
}

void DirectXApi::SetShadersFilesLocations(const std::string &path)
{
    shadersFileLocation_ = path + "DirectXApi/HLSL/";
}
void DirectXApi::SetShaderQuaility(GraphicsApi::ShaderQuaility)
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
std::vector<GraphicsApi::RendererType> DirectXApi::GetSupportedRenderers() const
{
    return {GraphicsApi::RendererType::SIMPLE};
}
GraphicsApi::IWindowApi &DirectXApi::GetWindowApi()
{
    return *windowApi_;
}
void DirectXApi::PrepareFrame()
{
    impl_->dxCondext_.devcon->ClearRenderTargetView(impl_->dxCondext_.renderTargetView, bgColor_);
    impl_->dxCondext_.devcon->ClearDepthStencilView(impl_->dxCondext_.depthStencilView,
                                                    D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
void DirectXApi::SetDefaultTarget()
{
}
void DirectXApi::SetBackgroundColor(const vec3 &bgColor)
{
    bgColor_[0] = bgColor.x;
    bgColor_[1] = bgColor.y;
    bgColor_[2] = bgColor.z;
    bgColor_[3] = 1.f;
}
void DirectXApi::EnableDepthTest()
{
    impl_->dxCondext_.devcon->OMSetRenderTargets(1, &impl_->dxCondext_.renderTargetView,
                                                 impl_->dxCondext_.depthStencilView);
}
void DirectXApi::DisableDepthTest()
{
    impl_->dxCondext_.devcon->OMSetRenderTargets(1, &impl_->dxCondext_.renderTargetView, nullptr);
}
GraphicsApi::ID DirectXApi::CreateShader(GraphicsApi::ShaderProgramType shaderType)
{
    if (not FileNameExist(shaderType))
    {
        return {};
    }

    auto filenames = GetDxForwardShaderFiles(shaderType);

    DxShader shader;

    auto vsShaderFileName = shadersFileLocation_ + filenames.at(GraphicsApi::ShaderType::VERTEX_SHADER);
    DEBUG_LOG("Compiling " + vsShaderFileName + "...");
    auto hr = CompileShaderFromFile(vsShaderFileName, "VS", "vs_4_0", &shader.blob_.vertex_);

    if (FAILED(hr))
    {
        auto failResult = "Vertex sheder can not be compiled. " + vsShaderFileName;
        MessageBox(NULL, failResult.c_str(), "Error", MB_OK);
        return {};
    }
    hr = impl_->dxCondext_.dev->CreateVertexShader(shader.blob_.vertex_->GetBufferPointer(),
                                                   shader.blob_.vertex_->GetBufferSize(), NULL, &shader.vertex_);
    if (FAILED(hr))
    {
        auto failResult = vsShaderFileName + " create vertex shader error";
        MessageBox(NULL, failResult.c_str(), __FUNCTION__, MB_OK);
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
    hr = impl_->dxCondext_.dev->CreateInputLayout(layout, numElements, shader.blob_.vertex_->GetBufferPointer(),
                                                  shader.blob_.vertex_->GetBufferSize(), &shader.vertexLayout_);
    if (FAILED(hr))
    {
        MessageBox(NULL,
                   "Can not create input layout."
                   "FX file.",
                   "Error", MB_OK);
        return {};
    }

    impl_->dxCondext_.devcon->IASetInputLayout(shader.vertexLayout_);

    auto fragmentShaderFile = shadersFileLocation_ + filenames.at(GraphicsApi::ShaderType::FRAGMENT_SHADER);
    DEBUG_LOG("Compiling " + fragmentShaderFile + "...");
    hr = CompileShaderFromFile(fragmentShaderFile, "PS", "ps_4_0", &shader.blob_.pixel_);

    if (FAILED(hr))
    {
        auto fail = "Fragment sheder can not be compiled. " + fragmentShaderFile;
        MessageBox(NULL, fail.c_str(), "Error", MB_OK);
        return {};
    }

    // Create the pixel shader
    hr = impl_->dxCondext_.dev->CreatePixelShader(shader.blob_.pixel_->GetBufferPointer(),
                                                  shader.blob_.pixel_->GetBufferSize(), NULL, &shader.pixel_);
    shader.blob_.Release();

    if (FAILED(hr))
    {
        MessageBox(NULL, "Can pixel shader error.", __FUNCTION__, MB_OK);
        return {};
    }

    impl_->shaders_.push_back(shader);
    return impl_->shaders_.size();
}
void DirectXApi::UseShader(uint32 id)
{
    if (id == 0)
    {
        return;
    }
    const auto &shader = impl_->shaders_[id - 1];

    impl_->dxCondext_.devcon->VSSetShader(shader.vertex_, NULL, 0);
    impl_->dxCondext_.devcon->PSSetShader(shader.pixel_, NULL, 0);
}

GraphicsApi::ID DirectXApi::CreateShaderBuffer(uint32 bindLocation, uint32 size)
{
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage          = D3D11_USAGE_DEFAULT;
    bd.ByteWidth      = size;
    bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;

    ID3D11Buffer *buffer;
    auto result = impl_->dxCondext_.dev->CreateBuffer(&bd, NULL, &buffer);

    if (FAILED(result))
    {
        MessageBox(NULL, "ID3D11Buffer create error.", "Error", MB_OK);
        return {};
    }

    impl_->buffers_.push_back({bindLocation, buffer});

    return impl_->buffers_.size() - 1;
}
void DirectXApi::UpdateShaderBuffer(uint32 id, void const *data)
{
    if (not Utils::ValidateRange(impl_->buffers_, id))
    {
        MessageBox(NULL, "ID3D11Buffer update error. Size exceeded", "Error", MB_OK);
        return;
    }
    const auto &buffer = impl_->buffers_[id];
    impl_->dxCondext_.devcon->UpdateSubresource(buffer.ptr, 0, NULL, data, 0, 0);
}
uint32 DirectXApi::BindShaderBuffer(uint32 id)
{
    if (not Utils::ValidateRange(impl_->buffers_, id))
    {
        MessageBox(NULL, "BindShaderBuffer ID3D11Buffer. Size exceeded", "Error", MB_OK);
        return 0;
    }
    const auto &buffer = impl_->buffers_[id];
    impl_->dxCondext_.devcon->VSSetConstantBuffers(buffer.bindLocation, 1, &buffer.ptr);
    // impl_->dxCondext_.devcon->PSSetConstantBuffers(buffer.bindLocation, 1, &buffer.ptr);

    return 0;  // to do return last binded buffer
}

std::optional<std::pair<D3D11_SAMPLER_DESC, ID3D11ShaderResourceView *>> CreateTexture2DDesc(ID3D11Device &dev,
                                                                                             const vec2ui &size,
                                                                                             void *data)
{
    ID3D11ShaderResourceView *rv;
    ID3D11Texture2D *texture2d;
    // D3DX11CreateShaderResourceViewFromFile(impl_->dxCondext_.dev, "seafloor.dds", NULL, NULL, &rv, NULL);

    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width     = size.x;
    desc.Height    = size.y;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    // desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    desc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage            = D3D11_USAGE_DEFAULT;
    desc.BindFlags        = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags   = 0;

    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem          = data;
    subResource.SysMemPitch      = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    auto result                  = dev.CreateTexture2D(&desc, &subResource, &texture2d);

    if (FAILED(result))
    {
        ERROR_LOG("Create CreateTexture2D failed.");
        return {};
    }

    // Create texture view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels       = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    result                            = dev.CreateShaderResourceView(texture2d, &srvDesc, &rv);
    texture2d->Release();

    if (FAILED(result))
    {
        ERROR_LOG("Create shaderResourceView failed.");
        return {};
    }

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD         = 0;
    sampDesc.MaxLOD         = D3D11_FLOAT32_MAX;
    sampDesc.MipLODBias     = 0;

    return std::make_pair(sampDesc, rv);
}

GraphicsApi::ID DirectXApi::CreateTexture(GraphicsApi::TextureType type, GraphicsApi::TextureFilter,
                                          GraphicsApi::TextureMipmap, GraphicsApi::BufferAtachment, vec2ui size,
                                          void *data)
{
    if (type != GraphicsApi::TextureType::U8_RGBA)
        return {};

    auto result = CreateTexture2DDesc(*impl_->dxCondext_.dev, size, data);

    if (not result)
        return {};

    return impl_->CreateTexture(result->first, result->second);
}
std::optional<uint32> DirectXApi::CreateTextureStorage(GraphicsApi::TextureType, GraphicsApi::TextureFilter, int32)
{
    return std::optional<uint32>();
}
GraphicsApi::ID DirectXApi::CreateCubMapTexture(vec2ui, std::vector<void *>)
{
    return {};
}
void DirectXApi::UpdateTexture(uint32, const vec2ui &, const vec2ui &, void *data)
{
}
void DirectXApi::UpdateTexture(uint32 id, const vec2ui &size, void *data)
{
    // TO DO: maybe is better way to update texture than delete and create new one

    auto &texture = impl_->GetTexture(id);
    texture.Release();

    auto result = CreateTexture2DDesc(*impl_->dxCondext_.dev, size, data);

    if (not result)
        return;

    texture = impl_->CreateDirectXTexture(result->first, result->second);
}
void DirectXApi::SetBuffers(const std::vector<GraphicsApi::BufferAtachment> &)
{
}
void DirectXApi::ClearBuffer(GraphicsApi::BufferType type)
{
    FLOAT color[] = {0.0f, 0.2f, 0.4f, 1.0f};
    if (type == GraphicsApi::BufferType::COLOR)
        impl_->dxCondext_.devcon->ClearRenderTargetView(impl_->dxCondext_.renderTargetView, color);

    if (type == GraphicsApi::BufferType::DEPTH)
        impl_->dxCondext_.devcon->ClearDepthStencilView(impl_->dxCondext_.depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void DirectXApi::ClearBuffers(const std::vector<GraphicsApi::BufferType> &)
{
}
void DirectXApi::EnableBlend()
{
    float bf[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    impl_->dxCondext_.devcon->OMSetBlendState(impl_->alphaBlendState, bf, 0xffffffff);
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

    const auto &texture = impl_->GetTexture(id);

    impl_->dxCondext_.devcon->PSSetShaderResources(0, 1, &texture.resourceView_);
    impl_->dxCondext_.devcon->PSSetSamplers(0, 1, &texture.samplerState_);
}
void DirectXApi::ActiveTexture(uint32 nr, uint32 id)
{
    if (id == 0)
        return;

    const auto &texture = impl_->GetTexture(id);
    impl_->dxCondext_.devcon->PSSetShaderResources(nr, 1, &texture.resourceView_);
    impl_->dxCondext_.devcon->PSSetSamplers(nr, 1, &texture.samplerState_);
}
GraphicsApi::ID DirectXApi::CreateBuffer()
{
    return {};
}
void DirectXApi::BindBuffer(GraphicsApi::BindType, uint32)
{
}
void DirectXApi::DeleteObject(uint32)
{
}
void DirectXApi::DeleteShaderBuffer(uint32)
{
}
std::string DirectXApi::GetBufferStatus()
{
    return std::string();
}
GraphicsApi::ID DirectXApi::CreatePatchMesh(const std::vector<float> &)
{
    return {};
}
GraphicsApi::ID DirectXApi::CreatePurePatchMeshInstanced(uint32, uint32)
{
    return {};
}
GraphicsApi::ID DirectXApi::CreateMesh(const GraphicsApi::MeshRawData &meshData, GraphicsApi::RenderType rendertype)
{
    Vao vao;
    vao.vertexes_.reserve(meshData.positions_.size() / 3);

    for (size_t x = 0; x < meshData.positions_.size(); x += 3)
    {
        Vertex v;
        v.position.x = meshData.positions_[x];
        v.position.y = meshData.positions_[x + 1];
        v.position.z = meshData.positions_[x + 2];

        if (not meshData.normals_.empty())
        {
            v.normal.x = meshData.normals_[x];
            v.normal.y = meshData.normals_[x + 1];
            v.normal.z = meshData.normals_[x + 2];
        }
        else
        {
            v.normal = vec3(0, 1, 0);
        }

        if (not meshData.tangents_.empty())
        {
            v.tangent.x = meshData.tangents_[x];
            v.tangent.y = meshData.tangents_[x + 1];
            v.tangent.z = meshData.tangents_[x + 2];
        }
        else
        {
            v.tangent = vec3(0);
        }

        vao.vertexes_.push_back(v);
    }

    int i = 0;
    for (size_t x = 0; x < meshData.textCoords_.size(); x += 2)
    {
        vao.vertexes_[i++].textCoord = vec2(meshData.textCoords_[x], meshData.textCoords_[x + 1]);
    }

    i = 0;
    for (size_t x = 0; x < meshData.bonesWeights_.size(); x += 4)
    {
        vao.vertexes_[i].weights = vec4(meshData.bonesWeights_[x], meshData.bonesWeights_[x + 1], meshData.bonesWeights_[x + 2], meshData.bonesWeights_[x + 3]);
        vao.vertexes_[i++].bonesIds = vec4i(meshData.joinIds_[x], meshData.joinIds_[x + 1], meshData.joinIds_[x + 2], meshData.joinIds_[x + 3]);
    }

    vao.indices_.reserve(meshData.indices_.size());

    for (auto i : meshData.indices_)
    {
        vao.indices_.push_back(i);
    }

    return impl_->CreateAndAddDxObject(vao);
}
GraphicsApi::ID DirectXApi::CreateDynamicLineMesh()
{
    return GraphicsApi::ID();
}
GraphicsApi::ID DirectXApi::CreateParticle()
{
    return {};
}
GraphicsApi::ID DirectXApi::CreateAnimatedParticle()
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
void DirectXApi::RenderTriangleStripMesh(uint32 id)
{
    if (id == 0)
        return;

    impl_->SetPrimitivTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    impl_->GetDxObject(id).Draw();
}
void DirectXApi::RenderMeshInstanced(uint32, uint32)
{
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
void DirectXApi::EnableCulling()
{
}
void DirectXApi::DisableCulling()
{
}
void DirectXApi::SetViewPort(uint32, uint32, uint32, uint32)
{
}
void DirectXApi::BindTexture(uint32 id) const
{
    if (id == 0)
        return;

    const auto &texture = impl_->GetTexture(id);
    impl_->dxCondext_.devcon->PSSetShaderResources(0, 1, &texture.resourceView_);
    impl_->dxCondext_.devcon->PSSetSamplers(0, 1, &texture.samplerState_);
}
void DirectXApi::BindImageTexture(uint32, GraphicsApi::TextureAccess)
{
}
GraphicsApi::ID DirectXApi::CreateShadowMap(uint32, uint32)
{
    return {};
}
void DirectXApi::PolygonModeRender()
{
}
void DirectXApi::LineModeRender()
{
}
void DirectXApi::SetBlendFunction(GraphicsApi::BlendFunctionType)
{
}
void DirectXApi::UpdateMatrixes(uint32, const std::vector<mat4> &)
{
}
void DirectXApi::UpdateLineMesh(uint32, const GraphicsApi::LineMesh&)
{
}
void DirectXApi::UpdateOffset(uint32, const std::vector<vec4> &)
{
}
void DirectXApi::UpdateBlend(uint32, const std::vector<float> &)
{
}
GraphicsApi::ID DirectXApi::CloneImage(uint32)
{
    return {};
}
void DirectXApi::CreateFont(const std::string &)
{
}
void DirectXApi::PrintText(const std::string &, const vec2i &)
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
    return glm::transpose(m);
}
std::vector<uint8> DirectXApi::GetTextureData(uint32) const
{
    return std::vector<uint8>();
}
const GraphicsApi::TextureInfo &DirectXApi::GetTextureInfo(uint32) const
{
    return defaultTextureInfo;
}
void DirectXApi::TakeSnapshoot(const std::string &path) const
{
}
}  // namespace DirectX
