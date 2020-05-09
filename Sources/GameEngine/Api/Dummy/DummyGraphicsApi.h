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
    GraphicsApi::IWindowApi& GetWindowApi() override
    {
        return *dummyWindowApiPtr_;
    }
    void Init() override
    {
    }
    void SetShaderQuaility(GraphicsApi::ShaderQuaility) override
    {
    }
    void CreateContext() override
    {
    }
    void DeleteContext() override
    {
    }
    void PrintVersion() override
    {
    }
    std::vector<GraphicsApi::RendererType> GetSupportedRenderers() const override
    {
        return {GraphicsApi::RendererType::SIMPLE, GraphicsApi::RendererType::FULL};
    }
    bool IsTesselationSupported() const override
    {
        return false;
    }
    void SetDefaultTarget() override
    {
    }
    void SetBackgroundColor(const vec3&) override
    {
    }
    void EnableDepthTest() override
    {
    }
    void DisableDepthTest() override
    {
    }
    void PrepareFrame() override
    {
    }
    GraphicsApi::ID CreateShader(GraphicsApi::ShaderProgramType) override
    {
        auto programId = ++id;
        return programId;
    }

    GraphicsApi::ID CreateShaderBuffer(uint32, uint32) override
    {
        return ++id;
    }

    void UpdateShaderBuffer(uint32 id, void const* buffer) override
    {
    }

    uint32 BindShaderBuffer(uint32) override
    {
        return 0;
    }

    void UseShader(uint32) override
    {
    }
    std::optional<uint32> CreateTextureStorage(GraphicsApi::TextureType, GraphicsApi::TextureFilter, int32) override
    {
        return {};
    }
    GraphicsApi::ID CreateTexture(GraphicsApi::TextureType, GraphicsApi::TextureFilter, GraphicsApi::TextureMipmap,
                                  GraphicsApi::BufferAtachment, vec2ui, void* data) override
    {
        return ++id;
    }
    GraphicsApi::ID CreateCubMapTexture(vec2ui, std::vector<void*>) override
    {
        return ++id;
    }
    void UpdateTexture(uint32, const vec2ui&, const vec2ui&, void* data) override
    {
    }
    void UpdateTexture(uint32, const vec2ui&, void* data) override
    {
    }
    void SetBuffers(const std::vector<GraphicsApi::BufferAtachment>&) override
    {
    }
    void ClearBuffer(GraphicsApi::BufferType) override
    {
    }
    void ClearBuffers(const std::vector<GraphicsApi::BufferType>&) override
    {
    }
    void ClearTexture(uint32, const Color&) override
    {
    }
    void EnableBlend() override
    {
    }
    void DisableBlend() override
    {
    }
    void EnableDepthMask() override
    {
    }
    void DisableDepthMask() override
    {
    }
    void ActiveTexture(uint32, uint32) override
    {
    }
    void ActiveTexture(uint32) override
    {
    }
    GraphicsApi::ID CreateBuffer() override
    {
        return ++id;
    }
    void BindBuffer(GraphicsApi::BindType, uint32) override
    {
    }
    void DeleteObject(uint32) override
    {
    }
    void DeleteShaderBuffer(uint32) override
    {
    }
    std::string GetBufferStatus() override
    {
        return {};
    }
    GraphicsApi::ID CreatePatchMesh(const std::vector<float>&) override
    {
        return ++id;
    }
    GraphicsApi::ID CreatePurePatchMeshInstanced(uint32, uint32) override
    {
        return ++id;
    }
    GraphicsApi::ID CreateMesh(const GraphicsApi::MeshRawData&, GraphicsApi::RenderType) override
    {
        return ++id;
    }
    GraphicsApi::ID CreateDynamicLineMesh() override
    {
        return std::nullopt;
    }
    GraphicsApi::ID CreateParticle() override;
    GraphicsApi::ID CreateAnimatedParticle() override
    {
        return ++id;
    }
    void RenderPurePatchedMeshInstances(uint32) override
    {
    }
    void Compute(uint32, uint32, uint32) override
    {
    }
    void RenderMesh(uint32) override
    {
    }
    void RenderTriangleStripMesh(uint32) override
    {
    }
    void RenderMeshInstanced(uint32, uint32) override
    {
    }
    void RenderPoints(uint32) override
    {
    }
    void RenderQuad() override
    {
    }
    void RenderQuadTs()
    {
    }
    void EnableCulling() override
    {
    }
    void DisableCulling() override
    {
    }
    void SetViewPort(uint32, uint32, uint32, uint32) override
    {
    }
    void BindTexture(uint32) const override
    {
    }
    void BindImageTexture(uint32, GraphicsApi::TextureAccess) override
    {
    }
    GraphicsApi::ID CreateShadowMap(uint32, uint32) override
    {
        return ++id;
    }
    void PolygonModeRender() override
    {
    }
    void LineModeRender() override
    {
    }
    void SetBlendFunction(GraphicsApi::BlendFunctionType) override
    {
    }
    void UpdateMesh(uint32, const GraphicsApi::MeshRawData&, const std::set<VertexBufferObjects>&) override
    {
    }
    void UpdateLineMesh(uint32, const GraphicsApi::LineMesh&) override
    {
    }
    void UpdateMatrixes(uint32, const std::vector<mat4>&) override
    {
    }
    void UpdateOffset(uint32, const std::vector<vec4>&) override
    {
    }
    void UpdateBlend(uint32, const std::vector<float>&) override
    {
    }
    GraphicsApi::ID CloneImage(uint32) override
    {
        return ++id;
    }
    // temp
    void CreateFont(const std::string&) override
    {
    }
    void PrintText(const std::string&, const vec2i&) override
    {
    }
    void LoadViewMatrix(const mat4&) override
    {
    }
    void LoadProjectionMatrix(const mat4&) override
    {
    }
    void DrawLine(const vec3& color, const vec3& from, const vec3& to) override
    {
    }
    mat4 PrepareMatrixToLoad(const mat4& m) override
    {
        return m;
    }
    std::vector<uint8> GetTextureData(uint32) const override
    {
        return {};
    }
    const GraphicsApi::TextureInfo& GetTextureInfo(uint32) const override
    {
        return textureInfo_;
    }
    void TakeSnapshoot(const std::string&) const override
    {
    }

private:
    uint32 id;
    std::unique_ptr<GraphicsApi::IWindowApi> dummyWindowApiPtr_;
    GraphicsApi::TextureInfo textureInfo_;
};
typedef std::shared_ptr<DummyGraphicsApi> DummyGraphicsApiPtr;
}  // namespace GameEngine
