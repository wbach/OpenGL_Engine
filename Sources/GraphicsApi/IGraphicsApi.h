#pragma once
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include "BlendFunctionsTypes.h"
#include "BufferParamters.h"
#include "FunctionsTypes.h"
#include "RenderType.h"
#include "ShaderQuaility.h"
#include "ShaderTypes.h"
#include "ShadersTypes.h"
#include "TextureInfo.h"
#include "TextureParamters.h"
#include "Types.h"
#include "WindowApi.hpp"
#include "WindowType.h"

namespace GraphicsApi
{
const size_t MAX_BONES_PER_VERTEX          = 4;
const uint32 MAX_PARTICLE_INSTANCES        = 10000;
const uint32 PARTICLE_INSTANCE_DATA_LENGTH = 21;
struct MeshRawData;

typedef std::optional<uint32> ID;

class IGraphicsApi
{
public:
    virtual ~IGraphicsApi()                                               = default;
    virtual IWindowApi& GetWindowApi()                                    = 0;
    virtual void Init()                                                   = 0;
    virtual void SetShadersFilesLocations(const std::string& path)        = 0;
    virtual void SetShaderQuaility(ShaderQuaility)                        = 0;
    virtual void CreateContext()                                          = 0;
    virtual void DeleteContext()                                          = 0;
    virtual void PrintVersion()                                           = 0;
    virtual bool IsTesselationSupported() const                           = 0;
    virtual void SetDefaultTarget()                                       = 0;
    virtual void SetBackgroundColor(const vec3&)                          = 0;
    virtual void EnableDepthTest()                                        = 0;
    virtual void DisableDepthTest()                                       = 0;
    virtual void PrepareFrame()                                           = 0;
    virtual uint32 CreateShader(Shaders shaderType, GraphicsApiFunctions) = 0;
    virtual uint32 GetShaderVariableLocation(uint32, const std::string&)  = 0;
    virtual ID CreateShaderBuffer(uint32 bindLocation, uint32 size)       = 0;
    virtual void UpdateShaderBuffer(uint32 id, void* buffer)              = 0;
    virtual uint32 BindShaderBuffer(uint32)                               = 0;
    virtual void UseShader(uint32)                                        = 0;
    virtual void BindAttribute(uint32, uint32, const std::string&)        = 0;
    virtual void LoadValueToShader(uint32, int)                           = 0;
    virtual void LoadValueToShader(uint32, uint32)                        = 0;
    virtual void LoadValueToShader(uint32, float)                         = 0;
    virtual void LoadValueToShader(uint32, const vec2&)                   = 0;
    virtual void LoadValueToShader(uint32, const vec3&)                   = 0;
    virtual void LoadValueToShader(uint32, const vec4&)                   = 0;
    virtual void LoadValueToShader(uint32, const mat3&)                   = 0;
    virtual void LoadValueToShader(uint32, const mat4&)                   = 0;
    virtual void LoadValueToShader(uint32, const std::vector<float>&)     = 0;
    virtual void LoadValueToShader(uint32, const std::vector<vec3>&)      = 0;
    virtual void LoadValueToShader(uint32, const std::vector<mat4>&)      = 0;
    virtual uint32 CreateTexture(TextureType, TextureFilter, TextureMipmap, BufferAtachment, vec2ui, void* data) = 0;
    virtual std::optional<uint32> CreateTextureStorage(TextureType, TextureFilter, int32 N)                      = 0;
    virtual uint32 CreateCubMapTexture(vec2ui, std::vector<void*>)                                               = 0;
    virtual void UpdateTexture(uint32, const vec2ui&, const vec2ui&, void* data)                                 = 0;
    virtual void UpdateTexture(uint32, const vec2ui&, void* data)                                                = 0;
    virtual void SetBuffers(const std::vector<BufferAtachment>&)                                                 = 0;
    virtual void ClearBuffer(BufferType)                                                                         = 0;
    virtual void ClearBuffers(const std::vector<BufferType>&)                                                    = 0;
    virtual void EnableBlend()                                                                                   = 0;
    virtual void DisableBlend()                                                                                  = 0;
    virtual void EnableDepthMask()                                                                               = 0;
    virtual void DisableDepthMask()                                                                              = 0;
    virtual void ActiveTexture(uint32, uint32)                                                                   = 0;
    virtual void ActiveTexture(uint32)                                                                           = 0;
    virtual uint32 CreateBuffer()                                                                                = 0;
    virtual void BindBuffer(BindType, uint32)                                                                    = 0;
    virtual void DeleteObject(uint32)                                                                            = 0;
    virtual void DeleteShaderBuffer(uint32)                                                                      = 0;
    virtual std::string GetBufferStatus()                                                                        = 0;
    virtual uint32 CreatePatchMesh(const std::vector<float>&)                                                    = 0;
    virtual uint32 CreatePurePatchMeshInstanced(uint32, uint32)                                                  = 0;
    virtual uint32 CreateMesh(const MeshRawData&, RenderType type)                                               = 0;
    virtual uint32 CreateParticle()                                                                              = 0;
    virtual uint32 CreateAnimatedParticle()                                                                      = 0;
    virtual void Compute(uint32, uint32, uint32)                                                                 = 0;
    virtual void RenderPurePatchedMeshInstances(uint32)                                                          = 0;
    virtual void RenderMesh(uint32)                                                                              = 0;
    virtual void RenderTriangleStripMesh(uint32)                                                                 = 0;
    virtual void RenderMeshInstanced(uint32, uint32)                                                             = 0;
    virtual void RenderPoints(uint32)                                                                            = 0;
    virtual void RenderQuad()                                                                                    = 0;
    virtual void RenderQuadTs()                                                                                  = 0;
    virtual void EnableCulling()                                                                                 = 0;
    virtual void DisableCulling()                                                                                = 0;
    virtual void SetViewPort(uint32, uint32, uint32, uint32)                                                     = 0;
    virtual void BindTexture(uint32)                                                                             = 0;
    virtual void BindImageTexture(uint32, TextureAccess)                                                         = 0;
    virtual uint32 CreateShadowMap(uint32, uint32)                                                               = 0;
    virtual void PolygonModeRender()                                                                             = 0;
    virtual void LineModeRender()                                                                                = 0;
    virtual void SetBlendFunction(BlendFunctionType)                                                             = 0;
    virtual void UpdateMatrixes(uint32, const std::vector<mat4>&)                                                = 0;
    virtual void UpdateOffset(uint32, const std::vector<vec4>&)                                                  = 0;
    virtual void UpdateBlend(uint32, const std::vector<float>&)                                                  = 0;
    virtual uint32 CloneImage(uint32)                                                                            = 0;
    virtual void LoadViewMatrix(const mat4&)                                                                     = 0;
    virtual void LoadProjectionMatrix(const mat4&)                                                               = 0;
    virtual void DrawLine(const vec3& color, const vec3& from, const vec3& to)                                   = 0;
    virtual mat4 PrepareMatrixToLoad(const mat4&)                                                                = 0;
    virtual std::vector<uint8> GetTextureData(uint32)                                                            = 0;
    virtual const TextureInfo& GetTextureInfo(uint32)                                                            = 0;
    // temp
    virtual void CreateFont(const std::string&)              = 0;
    virtual void PrintText(const std::string&, const vec2i&) = 0;
};
}  // namespace GraphicsApi
