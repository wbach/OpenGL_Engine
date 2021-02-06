#pragma once
#include "DummyWindowApi.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GraphicsApi/ShaderFiles.h"

namespace GameEngine
{
class DummyFrameBuffer : public GraphicsApi::IFrameBuffer
{
public:
    bool Init()
    {
        return true;
    }
    void Bind(GraphicsApi::FrameBuffer::BindType)
    {
    }
    void UnBind()
    {
    }
    void Clear()
    {
    }
    void CleanUp()
    {
    }
    GraphicsApi::ID GetAttachmentTexture(GraphicsApi::FrameBuffer::Type) const
    {
        return 0;
    }
    void TakeSnapshot(const std::string&)
    {
    }
};
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
    void SetBackgroundColor(const Color&) override
    {
    }
    const Color& GetBackgroundColor() const override
    {
        return bgColor;
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
    GraphicsApi::ID CreateTextureStorage(GraphicsApi::TextureType, GraphicsApi::TextureFilter, int32) override
    {
        return {};
    }
    GraphicsApi::ID CreateTexture(const GraphicsApi::Image&, GraphicsApi::TextureFilter,
                                  GraphicsApi::TextureMipmap) override
    {
        return ++id;
    }
    GraphicsApi::ID CreateCubMapTexture(const std::array<GraphicsApi::Image, 6>&) override
    {
        return ++id;
    }
    void UpdateTexture(uint32, const vec2ui&, const GraphicsApi::Image&) override
    {
    }
    void UpdateTexture(uint32, const GraphicsApi::Image&) override
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
    void RenderQuadTs() override
    {
    }
    void EnableClipingPlane(uint32) override
    {
    }
    void DisableCliping(uint32) override
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
    void BindDefaultFrameBuffer() override
    {
    }
    void SetShadersFilesLocations(const std::string&) override
    {
    }
    void RenderDebugNormals(uint32) override
    {
    }
    GraphicsApi::IFrameBuffer& CreateFrameBuffer(const std::vector<GraphicsApi::FrameBuffer::Attachment>&) override
    {
        return dummyFrameBuffer_;
    }
    void DeleteFrameBuffer(GraphicsApi::IFrameBuffer&) override
    {
    }

private:
    Color bgColor{0.f};
    uint32 id{0};
    std::unique_ptr<GraphicsApi::IWindowApi> dummyWindowApiPtr_{nullptr};
    GraphicsApi::TextureInfo textureInfo_{};
    DummyFrameBuffer dummyFrameBuffer_{};
};
}  // namespace GameEngine
