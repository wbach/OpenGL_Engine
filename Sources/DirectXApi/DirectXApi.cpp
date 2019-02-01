#include "DirectXApi.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include <string>
#include <xnamath.h>

#undef CreateFont

namespace DirectX
{
struct DirectXApi::Pimpl
{

};
DirectXApi::DirectXApi()
{
}
DirectXApi::DirectXApi(GraphicsApi::IWindowApiPtr windowApi)
{
}
DirectXApi::~DirectXApi()
{
}
void DirectXApi::Init()
{
}
void DirectXApi::SetShadersFilesLocations(const std::string &path)
{
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
    return GraphicsApi::IWindowApiPtr();
}
void DirectXApi::PrepareFrame()
{
}
void DirectXApi::SetDefaultTarget()
{
}
void DirectXApi::SetBackgroundColor(const vec3 &)
{
}
void DirectXApi::EnableDepthTest()
{
}
void DirectXApi::DisableDepthTest()
{
}
uint32 DirectXApi::CreateShader(GraphicsApi::Shaders shaderType, GraphicsApi::GraphicsApiFunctions)
{
    return uint32();
}
void DirectXApi::UseShader(uint32)
{
}
uint32 DirectXApi::GetShaderVariableLocation(uint32, const std::string &)
{
    return uint32();
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
void DirectXApi::ClearBuffer(GraphicsApi::BufferType)
{
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
std::string DirectXApi::GetBufferStatus()
{
    return std::string();
}
uint32 DirectXApi::CreatePurePatchMeshInstanced(uint32, uint32)
{
    return uint32();
}
uint32 DirectXApi::CreateMesh(const GraphicsApi::MeshRawData &)
{
    return uint32();
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
void DirectXApi::RenderMesh(uint32)
{
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
