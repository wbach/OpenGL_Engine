#pragma once
#include "DummyWindowApi.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GraphicsApi/ShaderFiles.h"

namespace GameEngine
{
class DummyGraphicsApi : public GraphicsApi::IGraphicsApi
{
public:
    DummyGraphicsApi()
    {
        dummyWindowApiPtr_ = std::make_unique<DummyWindowApi>();
    }
    virtual GraphicsApi::IWindowApi& GetWindowApi() override
    {
        return *dummyWindowApiPtr_;
    }
    virtual void Init() override
    {
    }
    virtual void SetShaderQuaility(GraphicsApi::ShaderQuaility) override
    {
    }
    virtual void CreateContext() override
    {
    }
    virtual void DeleteContext() override
    {
    }
    virtual void PrintVersion() override
    {
    }
    virtual bool IsTesselationSupported() const override
    {
        return false;
    }
    virtual void SetDefaultTarget() override
    {
    }
    virtual void SetBackgroundColor(const vec3&) override
    {
    }
    virtual void EnableDepthTest() override
    {
    }
    virtual void DisableDepthTest() override
    {
    }
    virtual void PrepareFrame() override
    {
    }
    virtual uint32 CreateShader(GraphicsApi::Shaders, GraphicsApi::GraphicsApiFunctions functions) override
    {
        auto programId = ++id;

        if (functions.count(GraphicsApi::GraphicFunctionType::SHADER_SET_ID) != 0)
            functions.at(GraphicsApi::GraphicFunctionType::SHADER_SET_ID)(programId);
        if (functions.count(GraphicsApi::GraphicFunctionType::SHADER_BIND_ATTRIBUTES) != 0)
            functions.at(GraphicsApi::GraphicFunctionType::SHADER_BIND_ATTRIBUTES)(0);
        if (functions.count(GraphicsApi::GraphicFunctionType::SHADER_VARIABLES_LOCATION) != 0)
            functions.at(GraphicsApi::GraphicFunctionType::SHADER_VARIABLES_LOCATION)(0);
        if (functions.count(GraphicsApi::GraphicFunctionType::SHADER_CONNECT_TEXTURES) != 0)
            functions.at(GraphicsApi::GraphicFunctionType::SHADER_CONNECT_TEXTURES)(0);

        return programId;
    }
    virtual uint32 GetShaderVariableLocation(uint32, const std::string&) override
    {
        return ++id;
    }

    virtual GraphicsApi::ID CreateShaderBuffer(uint32, uint32) override
    {
        return ++id;
    }

    virtual void UpdateShaderBuffer(uint32 id, void* buffer) override
    {
    }

    virtual uint32 BindShaderBuffer(uint32) override
    {
        return 0;
    }

    virtual void UseShader(uint32) override
    {
    }
    virtual void BindAttribute(uint32, uint32, const std::string&) override
    {
    }
    virtual void LoadValueToShader(uint32, int) override
    {
    }
    virtual void LoadValueToShader(uint32, uint32) override
    {
    }
    virtual void LoadValueToShader(uint32, float) override
    {
    }
    virtual void LoadValueToShader(uint32, const vec2&) override
    {
    }
    virtual void LoadValueToShader(uint32, const vec3&) override
    {
    }
    virtual void LoadValueToShader(uint32, const vec4&) override
    {
    }
    virtual void LoadValueToShader(uint32, const mat3&) override
    {
    }
    virtual void LoadValueToShader(uint32, const mat4&) override
    {
    }
    virtual void LoadValueToShader(uint32, const std::vector<float>&)
    {
    }
    virtual void LoadValueToShader(uint32, const std::vector<vec3>&)
    {
    }
    virtual std::optional<uint32> CreateTextureStorage(GraphicsApi::TextureType, GraphicsApi::TextureFilter,
                                                       int32) override
    {
        return {};
    }
    virtual uint32 CreateTexture(GraphicsApi::TextureType, GraphicsApi::TextureFilter, GraphicsApi::TextureMipmap,
                                 GraphicsApi::BufferAtachment, vec2ui, void* data) override
    {
        return ++id;
    }
    virtual uint32 CreateCubMapTexture(vec2ui, std::vector<void*>) override
    {
        return ++id;
    }
    virtual void UpdateTexture(uint32, const vec2ui&, const vec2ui&, void* data) override
    {
    }
    virtual void UpdateTexture(uint32, const vec2ui&, void* data) override
    {
    }
    virtual void SetBuffers(const std::vector<GraphicsApi::BufferAtachment>&) override
    {
    }
    virtual void ClearBuffer(GraphicsApi::BufferType) override
    {
    }
    virtual void ClearBuffers(const std::vector<GraphicsApi::BufferType>&) override
    {
    }
    virtual void EnableBlend() override
    {
    }
    virtual void DisableBlend() override
    {
    }
    virtual void EnableDepthMask() override
    {
    }
    virtual void DisableDepthMask() override
    {
    }
    virtual void ActiveTexture(uint32, uint32) override
    {
    }
    virtual void ActiveTexture(uint32) override
    {
    }
    virtual uint32 CreateBuffer() override
    {
        return ++id;
    }
    virtual void BindBuffer(GraphicsApi::BindType, uint32) override
    {
    }
    virtual void DeleteObject(uint32) override
    {
    }
    virtual void DeleteShaderBuffer(uint32) override
    {
    }
    virtual std::string GetBufferStatus() override
    {
        return {};
    }
    virtual uint32 CreatePatchMesh(const std::vector<float>&) override
    {
        return ++id;
    }
    virtual uint32 CreatePurePatchMeshInstanced(uint32, uint32) override
    {
        return ++id;
    }
    virtual uint32 CreateMesh(const GraphicsApi::MeshRawData&, GraphicsApi::RenderType) override
    {
        return ++id;
    }
    virtual uint32 CreateParticle() override;
    virtual uint32 CreateAnimatedParticle() override
    {
        return ++id;
    }
    virtual void RenderPurePatchedMeshInstances(uint32) override
    {
    }
    virtual void Compute(uint32, uint32, uint32) override
    {
    }
    virtual void RenderMesh(uint32) override
    {
    }
    virtual void RenderTriangleStripMesh(uint32) override
    {
    }
    virtual void RenderMeshInstanced(uint32, uint32) override
    {
    }
    virtual void RenderPoints(uint32) override
    {
    }
    virtual void RenderQuad() override
    {
    }
    virtual void RenderQuadTs()
    {
    }
    virtual void EnableCulling() override
    {
    }
    virtual void DisableCulling() override
    {
    }
    virtual void SetViewPort(uint32, uint32, uint32, uint32) override
    {
    }
    virtual void BindTexture(uint32) const override
    {
    }
    virtual void BindImageTexture(uint32, GraphicsApi::TextureAccess) override
    {
    }
    virtual uint32 CreateShadowMap(uint32, uint32) override
    {
        return ++id;
    }
    virtual void PolygonModeRender() override
    {
    }
    virtual void LineModeRender() override
    {
    }
    virtual void SetBlendFunction(GraphicsApi::BlendFunctionType) override
    {
    }
    virtual void UpdateMatrixes(uint32, const std::vector<mat4>&) override
    {
    }
    virtual void UpdateOffset(uint32, const std::vector<vec4>&) override
    {
    }
    virtual void UpdateBlend(uint32, const std::vector<float>&) override
    {
    }
    virtual uint32 CloneImage(uint32) override
    {
        return ++id;
    }
    // temp
    virtual void CreateFont(const std::string&) override
    {
    }
    virtual void PrintText(const std::string&, const vec2i&) override
    {
    }
    virtual void LoadViewMatrix(const mat4&) override
    {
    }
    virtual void LoadProjectionMatrix(const mat4&) override
    {
    }
    virtual void DrawLine(const vec3& color, const vec3& from, const vec3& to) override
    {
    }
    virtual mat4 PrepareMatrixToLoad(const mat4& m) override
    {
        return m;
    }
    virtual std::vector<uint8> GetTextureData(uint32) const override
    {
        return {};
    }
    virtual const GraphicsApi::TextureInfo& GetTextureInfo(uint32) const override
    {
        return textureInfo_;
    }
    virtual void TakeSnapshoot(const std::string&) const override
    {
    }

private:
    uint32 id;
    std::unique_ptr<GraphicsApi::IWindowApi> dummyWindowApiPtr_;
    GraphicsApi::TextureInfo textureInfo_;
};
typedef std::shared_ptr<DummyGraphicsApi> DummyGraphicsApiPtr;
}  // namespace GameEngine
