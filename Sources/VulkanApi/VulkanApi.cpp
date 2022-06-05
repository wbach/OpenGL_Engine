#include "VulkanApi.h"

#include <Utils/IdPool.h>

using namespace GraphicsApi;

namespace VulkanApi
{
namespace
{
const GraphicsApi::TextureInfo defaultTextureInfo;
}

struct VulkanApi::Pimpl
{
    Pimpl()
    {
    }
    Utils::IdPool idPool_;
    std::unique_ptr<GraphicsApi::IWindowApi> windowApi_;
    std::vector<std::unique_ptr<IFrameBuffer>> frameBuffers_;
};

VulkanApi::VulkanApi()
{
    impl_ = std::make_unique<VulkanApi::Pimpl>();
}

IWindowApi& VulkanApi::GetWindowApi()
{
    return *impl_->windowApi_;
}
void VulkanApi::Init()
{
}
void VulkanApi::SetShadersFilesLocations(const std::string&)
{
}
void VulkanApi::SetShaderQuaility(ShaderQuaility)
{
}
void VulkanApi::CreateContext()
{
}
void VulkanApi::DeleteContext()
{
}
void VulkanApi::PrintVersion()
{
}
bool VulkanApi::IsTesselationSupported() const
{
    return false;
}
std::vector<RendererType> VulkanApi::GetSupportedRenderers() const
{
    return {};
}
void VulkanApi::SetDefaultTarget()
{
}
void VulkanApi::SetBackgroundColor(const Color& color)
{
    bgColor_ = color;
}
const Color& VulkanApi::GetBackgroundColor() const
{
    return bgColor_;
}
void VulkanApi::EnableDepthTest()
{
}
void VulkanApi::DisableDepthTest()
{
}
void VulkanApi::PrepareFrame()
{
}
ID VulkanApi::CreateShader(ShaderProgramType)
{
    return {};
}
ID VulkanApi::CreateShaderBuffer(uint32 bindLocation, uint32 size)
{
    return {};
}
void VulkanApi::UpdateShaderBuffer(uint32 id, void const* buffer)
{
}
uint32 VulkanApi::BindShaderBuffer(uint32)
{
    return {};
}
void VulkanApi::UseShader(uint32)
{
}
ID VulkanApi::CreateTexture(const Utils::Image&, TextureFilter, TextureMipmap)
{
    return {};
}
ID VulkanApi::CreateTextureStorage(TextureType, TextureFilter, int32 N)
{
    return {};
}
ID VulkanApi::CreateCubMapTexture(const std::array<Utils::Image, 6>&)
{
    return {};
}
void VulkanApi::UpdateTexture(uint32, const vec2ui&, const Utils::Image&)
{
}
void VulkanApi::UpdateTexture(uint32, const Utils::Image&)
{
}
void VulkanApi::ClearTexture(uint32, const Color&)
{
}
void VulkanApi::EnableBlend()
{
}
void VulkanApi::DisableBlend()
{
}
void VulkanApi::EnableDepthMask()
{
}
void VulkanApi::DisableDepthMask()
{
}
void VulkanApi::ActiveTexture(uint32, uint32)
{
}
void VulkanApi::ActiveTexture(uint32)
{
}
void VulkanApi::DeleteObject(uint32)
{
}
void VulkanApi::DeleteShaderBuffer(uint32)
{
}
std::string VulkanApi::GetBufferStatus()
{
    return {};
}
ID VulkanApi::CreatePatchMesh(const std::vector<float>&)
{
    return {};
}
ID VulkanApi::CreatePurePatchMeshInstanced(uint32, uint32)
{
    return {};
}
ID VulkanApi::CreateMesh(const MeshRawData&, RenderType)
{
    return {};
}
ID VulkanApi::CreateDynamicLineMesh()
{
    return {};
}
ID VulkanApi::CreateParticle()
{
    return {};
}
ID VulkanApi::CreateAnimatedParticle()
{
    return {};
}
void VulkanApi::Compute(uint32, uint32, uint32)
{
}
void VulkanApi::RenderPurePatchedMeshInstances(uint32)
{
}
void VulkanApi::RenderMesh(uint32)
{
}
void VulkanApi::RenderDebugNormals(uint32)
{
}
void VulkanApi::RenderTriangleStripMesh(uint32)
{
}
void VulkanApi::RenderMeshInstanced(uint32, uint32)
{
}
void VulkanApi::RenderPoints(uint32)
{
}
void VulkanApi::RenderQuad()
{
}
void VulkanApi::RenderQuadTs()
{
}
void VulkanApi::EnableClipingPlane(uint32)
{
}
void VulkanApi::DisableCliping(uint32)
{
}
void VulkanApi::EnableCulling()
{
}
void VulkanApi::DisableCulling()
{
}
void VulkanApi::SetViewPort(uint32, uint32, uint32, uint32)
{
}
void VulkanApi::BindTexture(uint32) const
{
}
void VulkanApi::BindImageTexture(uint32, TextureAccess)
{
}
ID VulkanApi::CreateShadowMap(uint32, uint32)
{
    return {};
}
void VulkanApi::PolygonModeRender()
{
}
void VulkanApi::LineModeRender()
{
}
void VulkanApi::SetBlendFunction(BlendFunctionType)
{
}
void VulkanApi::UpdateMatrixes(uint32, const std::vector<mat4>&)
{
}
void VulkanApi::UpdateMesh(uint32, const MeshRawData&, const std::set<VertexBufferObjects>&)
{
}
void VulkanApi::UpdateLineMesh(uint32, const LineMesh&)
{
}
void VulkanApi::UpdateOffset(uint32, const std::vector<vec4>&)
{
}
void VulkanApi::UpdateBlend(uint32, const std::vector<float>&)
{
}
void VulkanApi::LoadViewMatrix(const mat4&)
{
}
void VulkanApi::LoadProjectionMatrix(const mat4&)
{
}
void VulkanApi::DrawLine(const vec3& color, const vec3& from, const vec3& to)
{
}
mat4 VulkanApi::PrepareMatrixToLoad(const mat4&)
{
    return {};
}
std::vector<uint8> VulkanApi::GetTextureData(uint32) const
{
    return {};
}
const TextureInfo& VulkanApi::GetTextureInfo(uint32) const
{
    return defaultTextureInfo;
}
void VulkanApi::TakeSnapshoot(const std::string& path) const
{
}

void VulkanApi::BindDefaultFrameBuffer()
{
}
IFrameBuffer& VulkanApi::CreateFrameBuffer(const std::vector<FrameBuffer::Attachment>&)
{
    return *impl_->frameBuffers_.back();
}
void VulkanApi::DeleteFrameBuffer(IFrameBuffer&)
{
}
}  // namespace VulkanApi
