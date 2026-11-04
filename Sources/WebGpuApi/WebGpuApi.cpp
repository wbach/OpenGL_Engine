#include "WebGpuApi.h"

#include <Logger/Log.h>

#include <stdexcept>

#include "DefaultFrameBuffer.h"
#include "WebGpuWindowApi.h"

namespace GraphicsApi::WebGpu
{
static DefaultFrameBuffer defaultFrameBuffer;

WebGpuApi::WebGpuApi()
    : windowApi{std::make_unique<WebGpuWindowApi>()}
{
}
WebGpuApi::~WebGpuApi()
{
}

IWindowApi& WebGpuApi::GetWindowApi()
{
    return *windowApi;
}
void WebGpuApi::Init()
{
    LOG_DEBUG << "";
}
void WebGpuApi::PrepareFrame()
{
}
void WebGpuApi::EndFrame()
{
}
void WebGpuApi::SetShadersFilesLocations(const std::filesystem::path&)
{
}
void WebGpuApi::DebugNormalMeshGeneration(bool)
{
}
void WebGpuApi::SetShaderQuaility(ShaderQuaility)
{
}
void WebGpuApi::CreateContext()
{
}
void WebGpuApi::DeleteContext()
{
}
void WebGpuApi::PrintVersion()
{
}
bool WebGpuApi::IsTesselationSupported() const
{
    return {};
}
std::vector<RendererType> WebGpuApi::GetSupportedRenderers() const
{
    return {};
}
void WebGpuApi::SetDefaultTarget()
{
}
void WebGpuApi::SetBackgroundColor(const Color&)
{
}
const Color& WebGpuApi::GetBackgroundColor() const
{
    throw std::logic_error("Not implemented");
}
void WebGpuApi::EnableDepthTest()
{
}
void WebGpuApi::DisableDepthTest()
{
}
ID WebGpuApi::CreateShader(ShaderProgramType)
{
    return {};
}
ID WebGpuApi::CreateShaderBuffer(uint32, uint32, DrawFlag)
{
    return {};
}
ID WebGpuApi::CreateShaderStorageBuffer(uint32, uint32, DrawFlag)
{
    return {};
}
void WebGpuApi::UpdateShaderBuffer(uint32, void const*)
{
}
void WebGpuApi::UpdateShaderStorageBuffer(uint32, void const*, uint32)
{
}
void* WebGpuApi::MapShaderStorageBuffer(uint32, uint32, uint32)
{
    return nullptr;
}
void WebGpuApi::UnmapShaderStorageBuffer(uint32)
{
}
uint32 WebGpuApi::BindShaderBuffer(uint32)
{
    return {};
}
void WebGpuApi::UseShader(uint32)
{
}
ID WebGpuApi::CreateTexture(const Utils::Image&, TextureFilter, TextureMipmap)
{
    return {};
}
ID WebGpuApi::CreateTexture(const std::vector<Utils::Image>&, GraphicsApi::TextureFilter, GraphicsApi::TextureMipmap)
{
    return {};
}
ID WebGpuApi::CreateTextureStorage(TextureType, TextureFilter, int32)
{
    return {};
}
ID WebGpuApi::CreateCubMapTexture(const CubemapImages&)
{
    return {};
}
std::optional<uint64_t> WebGpuApi::GetBindlessHandle(IdType)
{
    return {};
}
void WebGpuApi::GenerateMipmaps(IdType)
{
}
void WebGpuApi::CopyTexture(IdType, IdType, uint32, uint32)
{
}
void WebGpuApi::UpdateTexture(uint32, const vec2ui&, const Utils::Image&)
{
}
void WebGpuApi::UpdateTexture(uint32, const Utils::Image&)
{
}
void WebGpuApi::ClearTexture(uint32, const Color&)
{
}
void WebGpuApi::EnableBlend()
{
}
void WebGpuApi::DisableBlend()
{
}
void WebGpuApi::EnableDepthMask()
{
}
void WebGpuApi::DisableDepthMask()
{
}
void WebGpuApi::ActiveTexture(uint32, uint32)
{
}
void WebGpuApi::ActiveTexture(uint32)
{
}

void WebGpuApi::DeleteObject(uint32)
{
}
void WebGpuApi::DeleteObject(const std::vector<uint32>&)
{
}

void WebGpuApi::DeleteShaderBuffer(uint32)
{
}
void WebGpuApi::DeleteShaderBuffer(const std::vector<uint32>&)
{
}
std::string WebGpuApi::GetBufferStatus()
{
    return {};
}
ID WebGpuApi::CreatePatchMesh(const std::vector<float>&)
{
    return {};
}
ID WebGpuApi::CreatePurePatchMeshInstanced(uint32, uint32)
{
    return {};
}
ID WebGpuApi::CreateMesh(const MeshRawData&, RenderType)
{
    return {};
}

ID WebGpuApi::CreateDynamicLineMesh()
{
    return {};
}
ID WebGpuApi::CreateParticle()
{
    return {};
}
ID WebGpuApi::CreateAnimatedParticle()
{
    return {};
}

void WebGpuApi::Compute(uint32, uint32, uint32)
{
}

void WebGpuApi::RenderPurePatchedMeshInstances(uint32)
{
}
void WebGpuApi::RenderMesh(uint32)
{
}
void WebGpuApi::RenderProcedural(uint32)
{
}
void WebGpuApi::RenderDebugNormals(uint32)
{
}
void WebGpuApi::RenderTriangleStripMesh(uint32)
{
}
void WebGpuApi::RenderMeshInstanced(uint32, uint32)
{
}
void WebGpuApi::RenderPoints(uint32)
{
}
void WebGpuApi::RenderQuad()
{
}
void WebGpuApi::RenderQuadTs()
{
}
void WebGpuApi::EnableClipingPlane(uint32)
{
}
void WebGpuApi::DisableCliping(uint32)
{
}
void WebGpuApi::EnableCulling()
{
}
void WebGpuApi::DisableCulling()
{
}
void WebGpuApi::SetViewPort(uint32, uint32, uint32, uint32)
{
}
void WebGpuApi::BindTexture(uint32) const
{
}
void WebGpuApi::BindImageTexture(uint32, TextureAccess)
{
}
ID WebGpuApi::CreateShadowMap(uint32, uint32)
{
    return {};
}
void WebGpuApi::PolygonModeRender()
{
}
void WebGpuApi::LineModeRender()
{
}
void WebGpuApi::SetBlendFunction(BlendFunctionType)
{
}
void WebGpuApi::UpdateMatrixes(uint32, const std::vector<mat4>&)
{
}
void WebGpuApi::UpdateMesh(uint32, const MeshRawData&, const std::set<VertexBufferObjects>&)
{
}
void WebGpuApi::UpdateLineMesh(uint32, const LineMesh&)
{
}
void WebGpuApi::UpdateOffset(uint32, const std::vector<vec4>&)
{
}
void WebGpuApi::UpdateBlend(uint32, const std::vector<float>&)
{
}
void WebGpuApi::LoadViewMatrix(const mat4&)
{
}
void WebGpuApi::LoadProjectionMatrix(const mat4&)
{
}
void WebGpuApi::DrawLine(const vec3&, const vec3&, const vec3&)
{
}
mat4 WebGpuApi::PrepareMatrixToLoad(const mat4&)
{
    return {};
}
std::vector<uint8> WebGpuApi::GetTextureData(uint32) const
{
    return {};
}
const TextureInfo& WebGpuApi::GetTextureInfo(uint32) const
{
    static TextureInfo info;
    return info;
}
void WebGpuApi::TakeSnapshot(const std::filesystem::path&) const
{
}
std::optional<Utils::Image> WebGpuApi::GetImage(IdType) const
{
    return {};
}
std::vector<Utils::Image> WebGpuApi::GetImageArray(IdType) const
{
    return {};
}
IFrameBuffer& WebGpuApi::GetDefaultFrameBuffer()
{
    return defaultFrameBuffer;
}
IFrameBuffer& WebGpuApi::CreateFrameBuffer(const std::vector<FrameBuffer::Attachment>&)
{
    return defaultFrameBuffer;
}
void WebGpuApi::DeleteFrameBuffer(IFrameBuffer&)
{
}
}  // namespace GraphicsApi::WebGpu
