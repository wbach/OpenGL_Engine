#include "DirectXApi.h"
#include <D3D11Shader.h>
#include <d3d11.h>
#include <d3dx10.h>
#include <d3dx11.h>
#include <windows.h>
#include <windowsx.h>
#include <xnamath.h>
#include <string>
#include "DirectXContext.h"
#include "DirectXTools.h"
#include "DxShader.h"
#include "GraphicsApi/MeshRawData.h"
#include "Utils.h"
#include "Vao.h"
#include "WinApi/WinApi.h"

#undef CreateFont

namespace DirectX
{
struct Vertex
{
    vec3 position;
    vec2 textCoord;
};

struct DirectXApi::Pimpl
{
    DirectXContext dxCondext_;
    std::vector<Buffer> buffers_;
    std::vector<DxShader> shaders_;
    std::vector<Object> objects_;
};

DirectXApi::DirectXApi()
{
    impl_      = std::make_unique<Pimpl>();
    windowApi_ = std::make_shared<WinApi>(impl_->dxCondext_);

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
    InitRenderTarget();
    InitDepthSetncilView();
    impl_->dxCondext_.devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
void DirectXApi::InitRenderTarget()
{
    // get the address of the back buffer
    ID3D11Texture2D *pBackBuffer;
    impl_->dxCondext_.swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);

    // use the back buffer address to create the render target
    impl_->dxCondext_.dev->CreateRenderTargetView(pBackBuffer, NULL, &impl_->dxCondext_.backbuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    impl_->dxCondext_.devcon->OMSetRenderTargets(1, &impl_->dxCondext_.backbuffer, impl_->dxCondext_.depthStencilView);
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
        return;

    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format             = descDepth.Format;
    descDSV.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr                         = impl_->dxCondext_.dev->CreateDepthStencilView(impl_->dxCondext_.depthStencil, &descDSV,
                                                       &impl_->dxCondext_.depthStencilView);
    if (FAILED(hr))
        return;

    impl_->dxCondext_.devcon->OMSetRenderTargets(1, &impl_->dxCondext_.backbuffer, impl_->dxCondext_.depthStencilView);
}

void DirectXApi::SetShadersFilesLocations(const std::string &path)
{
    shadersFileLocation_ = path;
}
void DirectXApi::SetShaderQuaility(GraphicsApi::ShaderQuaility)
{
}
void DirectXApi::CreateContext()
{
}
void DirectXApi::DeleteContext()
{
}
void DirectXApi::PrintVersion()
{
}
GraphicsApi::IWindowApiPtr DirectXApi::GetWindowApi()
{
    return windowApi_;
}
void DirectXApi::PrepareFrame()
{
    impl_->dxCondext_.devcon->ClearRenderTargetView(impl_->dxCondext_.backbuffer, bgColor_);
    impl_->dxCondext_.devcon->ClearDepthStencilView(impl_->dxCondext_.depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
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
}
void DirectXApi::DisableDepthTest()
{
}
uint32 DirectXApi::CreateShader(GraphicsApi::Shaders shaderType, GraphicsApi::GraphicsApiFunctions)
{
    if (shaderType != GraphicsApi::Shaders::Entity)
    {
        return 0;
    }

    DxShader shader;

    auto hr = CompileShaderFromFile(shadersFileLocation_ + "SimpleShaders.fx", "VS", "vs_4_0", &shader.blob_.vertex_);
    if (FAILED(hr))
    {
        MessageBox(NULL,
                   "The FX file cannot be compiled.  Please run this executable from the directory that contains the "
                   "FX file.",
                   "Error", MB_OK);
        return 0;
    }
    hr = impl_->dxCondext_.dev->CreateVertexShader(shader.blob_.vertex_->GetBufferPointer(),
                                                   shader.blob_.vertex_->GetBufferSize(), NULL, &shader.vertex_);
    if (FAILED(hr))
    {
        shader.blob_.vertex_->Release();
        shader.blob_.vertex_ = nullptr;
        return 0;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
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
        return 0;
    }

    hr = CompileShaderFromFile(shadersFileLocation_ + "SimpleShaders.fx", "PS", "ps_4_0", &shader.blob_.pixel_);

    if (FAILED(hr))
    {
        MessageBox(NULL,
                   "The FX file cannot be compiled.  Please run this executable from the directory that contains the "
                   "FX file.",
                   "Error", MB_OK);
        return 0;
    }

    // Create the pixel shader
    hr = impl_->dxCondext_.dev->CreatePixelShader(shader.blob_.pixel_->GetBufferPointer(),
                                                  shader.blob_.pixel_->GetBufferSize(), NULL, &shader.pixel_);

    if (FAILED(hr))
    {
        shader.blob_.pixel_->Release();
        shader.blob_.pixel_ = nullptr;
        return 0;
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
uint32 DirectXApi::GetShaderVariableLocation(uint32, const std::string &)
{
    return uint32();
}
GraphicsApi::ID DirectXApi::CreateShaderBuffer(uint32 bindLocation, uint32 size)
{
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage          = D3D11_USAGE_DEFAULT;
    bd.ByteWidth      = size;
    bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.BindFlags      = 0;
    bd.MiscFlags      = 0;

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
void DirectXApi::UpdateShaderBuffer(uint32 id, void *data)
{
    if (not Utils::ValidateRange(impl_->buffers_, id))
    {
        MessageBox(NULL, "ID3D11Buffer update error. Size exceeded", "Error", MB_OK);
        return;
    }
    const auto &buffer = impl_->buffers_[id];
    impl_->dxCondext_.devcon->UpdateSubresource(buffer.ptr, 0, NULL, &data, 0, 0);
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

    return 0;  // to do return last binded buffer
}
void DirectXApi::BindAttribute(uint32, uint32, const std::string &)
{
}
void DirectXApi::LoadValueToShader(uint32, int)
{
}
void DirectXApi::LoadValueToShader(uint32, float)
{
}
void DirectXApi::LoadValueToShader(uint32, uint32)
{
}
void DirectXApi::LoadValueToShader(uint32, const vec2 &)
{
}
void DirectXApi::LoadValueToShader(uint32, const vec3 &)
{
}
void DirectXApi::LoadValueToShader(uint32, const vec4 &)
{
}
void DirectXApi::LoadValueToShader(uint32, const mat3 &)
{
}
void DirectXApi::LoadValueToShader(uint32, const mat4 &)
{
}
void DirectXApi::LoadValueToShader(uint32, const std::vector<float> &)
{
}
void DirectXApi::LoadValueToShader(uint32, const std::vector<vec3> &)
{
}
void DirectXApi::LoadValueToShader(uint32, const std::vector<mat4> &)
{
}
uint32 DirectXApi::CreateTexture(GraphicsApi::TextureType, GraphicsApi::TextureFilter, GraphicsApi::TextureMipmap,
                                 GraphicsApi::BufferAtachment, vec2ui, void *data)
{
    return uint32();
}
uint32 DirectXApi::CreateCubMapTexture(vec2ui, std::vector<void *>)
{
    return uint32();
}
void DirectXApi::SetBuffers(const std::vector<GraphicsApi::BufferAtachment> &)
{
}
void DirectXApi::ClearBuffer(GraphicsApi::BufferType type)
{
    FLOAT color[] = {0.0f, 0.2f, 0.4f, 1.0f};
    impl_->dxCondext_.devcon->ClearRenderTargetView(impl_->dxCondext_.backbuffer, color);

    if (type == GraphicsApi::BufferType::DEPTH)
        impl_->dxCondext_.devcon->ClearDepthStencilView(impl_->dxCondext_.depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void DirectXApi::ClearBuffers(const std::vector<GraphicsApi::BufferType> &)
{
}
void DirectXApi::EnableBlend()
{
}
void DirectXApi::DisableBlend()
{
}
void DirectXApi::EnableDepthMask()
{
}
void DirectXApi::DisableDepthMask()
{
}
void DirectXApi::ActiveTexture(uint32)
{
}
void DirectXApi::ActiveTexture(uint32, uint32)
{
}
uint32 DirectXApi::CreateBuffer()
{
    return uint32();
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
uint32 DirectXApi::CreatePurePatchMeshInstanced(uint32, uint32)
{
    return uint32();
}
uint32 DirectXApi::CreateMesh(const GraphicsApi::MeshRawData &meshData)
{
    Object obj;

    std::vector<Vertex> vertexes;
    vertexes.reserve(meshData.positions_.size() / 3.f);

    for (size_t x = 0; x < meshData.positions_.size(); x += 3)
    {
        Vertex v;
        v.position.x = meshData.positions_[x];
        v.position.y = meshData.positions_[x + 1];
        v.position.z = meshData.positions_[x + 2];
        vertexes.push_back(v);
    }

    int i = 0;
    for (size_t x = 0; x < meshData.textCoords_.size(); x += 2)
    {
        vertexes[i++].textCoord = vec2(meshData.textCoords_[x], meshData.textCoords_[x + 1]);
    }

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage          = D3D11_USAGE_DEFAULT;
    bd.ByteWidth      = sizeof(Vertex) * vertexes.size();
    bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = &vertexes[0];

    Buffer vertexBuffer;
    vertexBuffer.bindLocation = 0;
    auto hr                   = impl_->dxCondext_.dev->CreateBuffer(&bd, &InitData, &vertexBuffer.ptr);

    obj.buffers_[(int)VertexBufferObjects::POSITION] = vertexBuffer;

    // Set vertex buffer
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    impl_->dxCondext_.devcon->IASetVertexBuffers(vertexBuffer.bindLocation, 1, &vertexBuffer.ptr, &stride, &offset);

    bd.Usage          = D3D11_USAGE_DEFAULT;
    bd.ByteWidth      = sizeof(uint32) * meshData.indices_.size();
    bd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem  = &meshData.indices_[0];

    obj.indiciesSize = meshData.indices_.size();

    Buffer indiecies;
    hr = impl_->dxCondext_.dev->CreateBuffer(&bd, &InitData, &indiecies.ptr);
    if (FAILED(hr))
        return hr;
    obj.buffers_[(int)VertexBufferObjects::INDICES] = indiecies;
    // Set index buffer
    impl_->dxCondext_.devcon->IASetIndexBuffer(indiecies.ptr, DXGI_FORMAT_R16_UINT, 0);
    impl_->dxCondext_.devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    impl_->objects_.push_back(obj);
    return impl_->objects_.size();
}
uint32 DirectXApi::CreateParticle()
{
    return uint32();
}
uint32 DirectXApi::CreateAnimatedParticle()
{
    return uint32();
}
void DirectXApi::RenderPurePatchedMeshInstances(uint32)
{
}
void DirectXApi::RenderMesh(uint32 id)
{
    if (id == 0)
        return;

    const auto &mesh = impl_->objects_[id - 1];
    mesh.Bind(impl_->dxCondext_.devcon);

    impl_->dxCondext_.devcon->DrawIndexed(mesh.indiciesSize, 0, 0);
}
void DirectXApi::RenderTriangleStripMesh(uint32)
{
}
void DirectXApi::RenderMeshInstanced(uint32, uint32)
{
}
void DirectXApi::RenderPoints(uint32)
{
}
void DirectXApi::RenderQuad()
{
}
void DirectXApi::RenderQuadTs()
{
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
void DirectXApi::BindTexture(uint32)
{
}
uint32 DirectXApi::CreateShadowMap(uint32, uint32)
{
    return uint32();
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
void DirectXApi::UpdateOffset(uint32, const std::vector<vec4> &)
{
}
void DirectXApi::UpdateBlend(uint32, const std::vector<float> &)
{
}
uint32 DirectXApi::CloneImage(uint32)
{
    return uint32();
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
}  // namespace DirectX
