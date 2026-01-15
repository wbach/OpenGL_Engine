#pragma once
#include <Types.h>
#include <Utils/Image/Image.h>

#include <array>
#include <filesystem>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "BlendFunctionsTypes.h"
#include "BufferParamters.h"
#include "FunctionsTypes.h"
#include "GraphicsApiDef.h"
#include "IFrameBuffer.h"
#include "LineMesh.h"
#include "RenderType.h"
#include "RendererType.h"
#include "ShaderProgramType.h"
#include "ShaderQuaility.h"
#include "ShaderTypes.h"
#include "TextureInfo.h"
#include "TextureParamters.h"
#include "WindowApi.hpp"
#include "WindowType.h"

namespace GraphicsApi
{
struct MeshRawData;

using CubemapImages = std::array<Utils::Image, 6>;

enum class DrawFlag : uint32
{
    Static,
    Dynamic,
};

class IGraphicsApi
{
public:
    virtual ~IGraphicsApi() = default;

    virtual IWindowApi& GetWindowApi()                                                                                 = 0;
    virtual void Init()                                                                                                = 0;
    virtual void SetShadersFilesLocations(const std::filesystem::path&)                                                = 0;
    virtual void SetShaderQuaility(ShaderQuaility)                                                                     = 0;
    virtual void CreateContext()                                                                                       = 0;
    virtual void DeleteContext()                                                                                       = 0;
    virtual void PrintVersion()                                                                                        = 0;
    virtual bool IsTesselationSupported() const                                                                        = 0;
    virtual std::vector<RendererType> GetSupportedRenderers() const                                                    = 0;
    virtual void SetDefaultTarget()                                                                                    = 0;
    virtual void SetBackgroundColor(const Color&)                                                                      = 0;
    virtual const Color& GetBackgroundColor() const                                                                    = 0;
    virtual void EnableDepthTest()                                                                                     = 0;
    virtual void DisableDepthTest()                                                                                    = 0;
    virtual void PrepareFrame()                                                                                        = 0;
    virtual ID CreateShader(ShaderProgramType)                                                                         = 0;
    virtual ID CreateShaderBuffer(uint32 bindLocation, uint32 size, DrawFlag)                                          = 0;
    virtual ID CreateShaderStorageBuffer(uint32 bindLocation, uint32 size, DrawFlag)                                   = 0;
    virtual void UpdateShaderBuffer(uint32 id, void const* buffer)                                                     = 0;
    virtual void UpdateShaderStorageBuffer(uint32 id, void const* buffer, uint32 size)                                 = 0;
    virtual void* MapShaderStorageBuffer(uint32 id, uint32 size, uint32 flags)                                         = 0;
    virtual void UnmapShaderStorageBuffer(uint32 id)                                                                   = 0;
    virtual uint32 BindShaderBuffer(uint32)                                                                            = 0;
    virtual void UseShader(uint32)                                                                                     = 0;
    virtual ID CreateTexture(const Utils::Image&, TextureFilter, TextureMipmap)                                        = 0;
    virtual ID CreateTexture(const std::vector<Utils::Image>&, GraphicsApi::TextureFilter, GraphicsApi::TextureMipmap) = 0;
    virtual ID CreateTextureStorage(TextureType, TextureFilter, int32 N)                                               = 0;
    virtual ID CreateCubMapTexture(const CubemapImages&)                                                               = 0;
    virtual void GenerateMipmaps(IdType)                                                                               = 0;
    virtual void CopyTexture(IdType src, IdType dst, uint32 width, uint32 height)                                      = 0;
    virtual void UpdateTexture(uint32, const vec2ui&, const Utils::Image&)                                             = 0;
    virtual void UpdateTexture(uint32, const Utils::Image&)                                                            = 0;
    virtual void ClearTexture(uint32, const Color&)                                                                    = 0;
    virtual void EnableBlend()                                                                                         = 0;
    virtual void DisableBlend()                                                                                        = 0;
    virtual void EnableDepthMask()                                                                                     = 0;
    virtual void DisableDepthMask()                                                                                    = 0;
    virtual void ActiveTexture(uint32, uint32)                                                                         = 0;
    virtual void ActiveTexture(uint32)                                                                                 = 0;
    virtual void DeleteObject(uint32)                                                                                  = 0;
    virtual void DeleteShaderBuffer(uint32)                                                                            = 0;
    virtual std::string GetBufferStatus()                                                                              = 0;
    virtual ID CreatePatchMesh(const std::vector<float>&)                                                              = 0;
    virtual ID CreatePurePatchMeshInstanced(uint32, uint32)                                                            = 0;
    virtual ID CreateMesh(const MeshRawData&, RenderType)                                                              = 0;
    virtual ID CreateDynamicLineMesh()                                                                                 = 0;
    virtual ID CreateParticle()                                                                                        = 0;
    virtual ID CreateAnimatedParticle()                                                                                = 0;
    virtual void Compute(uint32, uint32, uint32)                                                                       = 0;
    virtual void RenderPurePatchedMeshInstances(uint32)                                                                = 0;
    virtual void RenderMesh(uint32)                                                                                    = 0;
    virtual void RenderProcedural(uint32)                                                                              = 0;
    virtual void RenderDebugNormals(uint32)                                                                            = 0;
    virtual void RenderTriangleStripMesh(uint32)                                                                       = 0;
    virtual void RenderMeshInstanced(uint32, uint32)                                                                   = 0;
    virtual void RenderPoints(uint32)                                                                                  = 0;
    virtual void RenderQuad()                                                                                          = 0;
    virtual void RenderQuadTs()                                                                                        = 0;
    virtual void EnableClipingPlane(uint32)                                                                            = 0;
    virtual void DisableCliping(uint32)                                                                                = 0;
    virtual void EnableCulling()                                                                                       = 0;
    virtual void DisableCulling()                                                                                      = 0;
    virtual void SetViewPort(uint32, uint32, uint32, uint32)                                                           = 0;
    virtual void BindTexture(uint32) const                                                                             = 0;
    virtual void BindImageTexture(uint32, TextureAccess)                                                               = 0;
    virtual ID CreateShadowMap(uint32, uint32)                                                                         = 0;
    virtual void PolygonModeRender()                                                                                   = 0;
    virtual void LineModeRender()                                                                                      = 0;
    virtual void SetBlendFunction(BlendFunctionType)                                                                   = 0;
    virtual void UpdateMatrixes(uint32, const std::vector<mat4>&)                                                      = 0;
    virtual void UpdateMesh(uint32, const MeshRawData&, const std::set<VertexBufferObjects>&)                          = 0;
    virtual void UpdateLineMesh(uint32, const LineMesh&)                                                               = 0;
    virtual void UpdateOffset(uint32, const std::vector<vec4>&)                                                        = 0;
    virtual void UpdateBlend(uint32, const std::vector<float>&)                                                        = 0;
    virtual void LoadViewMatrix(const mat4&)                                                                           = 0;
    virtual void LoadProjectionMatrix(const mat4&)                                                                     = 0;
    virtual void DrawLine(const vec3& color, const vec3& from, const vec3& to)                                         = 0;
    virtual mat4 PrepareMatrixToLoad(const mat4&)                                                                      = 0;
    virtual std::vector<uint8> GetTextureData(uint32) const                                                            = 0;
    virtual const TextureInfo& GetTextureInfo(uint32) const                                                            = 0;
    virtual void TakeSnapshoot(const std::string& path) const                                                          = 0;
    virtual std::optional<Utils::Image> GetImage(IdType) const                                                         = 0;

    virtual IFrameBuffer& GetDefaultFrameBuffer()                                        = 0;
    virtual IFrameBuffer& CreateFrameBuffer(const std::vector<FrameBuffer::Attachment>&) = 0;
    virtual void DeleteFrameBuffer(IFrameBuffer&)                                        = 0;
};
}  // namespace GraphicsApi
