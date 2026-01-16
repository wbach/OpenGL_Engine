#pragma once
#include <stack>

#include "GraphicsApi/IGraphicsApi.h"
#include "OpenGLMesh.h"
#include "Shaders/ShaderManager.h"
#include "SimpleModels/Quad.h"
#include "optional.hpp"

namespace OpenGLApi
{
const uint32 MAX_SHADER_BUFFER_SIZE = 20;

class OpenGLApi : public GraphicsApi::IGraphicsApi
{
public:
    using IFrameBuffer = GraphicsApi::IFrameBuffer;

    OpenGLApi();
    OpenGLApi(std::unique_ptr<GraphicsApi::IWindowApi>);
    ~OpenGLApi();
    void Init() override;
    void SetShadersFilesLocations(const std::filesystem::path&) override;
    void SetShaderQuaility(GraphicsApi::ShaderQuaility) override;
    void CreateContext() override;
    void DeleteContext() override;
    void PrintVersion() override;
    std::vector<GraphicsApi::RendererType> GetSupportedRenderers() const override;
    bool IsTesselationSupported() const override;
    GraphicsApi::IWindowApi& GetWindowApi() override;

    void PrepareFrame() override;
    void SetDefaultTarget() override;
    void SetBackgroundColor(const Color&) override;
    const Color& GetBackgroundColor() const override;
    void EnableDepthTest() override;
    void DisableDepthTest() override;

    GraphicsApi::ID CreateShader(GraphicsApi::ShaderProgramType) override;
    void UseShader(uint32) override;

    GraphicsApi::ID CreateShaderBuffer(uint32 bindLocation, uint32 size, GraphicsApi::DrawFlag) override;
    GraphicsApi::ID CreateShaderStorageBuffer(uint32 bindLocation, uint32 size, GraphicsApi::DrawFlag) override;
    void UpdateShaderBuffer(uint32 id, void const* buffer) override;
    void UpdateShaderStorageBuffer(uint32 id, void const* buffer, uint32 size) override;
    void* MapShaderStorageBuffer(uint32 id, uint32 size, uint32 flags) override;
    void UnmapShaderStorageBuffer(uint32 id) override;

    uint32 BindShaderBuffer(uint32) override;

    GraphicsApi::ID CreateTexture(const Utils::Image&, GraphicsApi::TextureFilter, GraphicsApi::TextureMipmap) override;
    GraphicsApi::ID CreateTexture(const std::vector<Utils::Image>&, GraphicsApi::TextureFilter,
                                  GraphicsApi::TextureMipmap) override;
    std::optional<uint32> CreateTextureStorage(GraphicsApi::TextureType, GraphicsApi::TextureFilter, int32 N) override;
    GraphicsApi::ID CreateCubMapTexture(const std::array<Utils::Image, 6>&) override;
    void GenerateMipmaps(IdType) override;
    void CopyTexture(IdType src, IdType dst, uint32 width, uint32 height) override;

    void UpdateTexture(uint32, const vec2ui&, const Utils::Image&) override;
    void UpdateTexture(uint32, const Utils::Image&) override;

    void ClearTexture(uint32, const Color&) override;

    void EnableBlend() override;
    void DisableBlend() override;

    void EnableDepthMask() override;
    void DisableDepthMask() override;
    void ActiveTexture(uint32) override;
    void ActiveTexture(uint32, uint32) override;

    void DeleteObject(uint32) override;
    void DeleteShaderBuffer(uint32) override;

    std::string GetBufferStatus() override;
    GraphicsApi::ID CreatePatchMesh(const std::vector<float>&) override;
    GraphicsApi::ID CreatePurePatchMeshInstanced(uint32, uint32) override;
    GraphicsApi::ID CreateMesh(const GraphicsApi::MeshRawData&, GraphicsApi::RenderType type) override;
    GraphicsApi::ID CreateDynamicLineMesh() override;
    GraphicsApi::ID CreateParticle() override;
    GraphicsApi::ID CreateAnimatedParticle() override;
    void Compute(uint32, uint32, uint32) override;
    void RenderPurePatchedMeshInstances(uint32) override;
    void RenderMesh(uint32) override;
    void RenderProcedural(uint32) override;
    void RenderTriangleStripMesh(uint32) override;
    void RenderMeshInstanced(uint32, uint32) override;
    void RenderDebugNormals(uint32) override;
    void RenderPoints(uint32) override;
    void RenderQuad() override;
    void RenderQuadTs() override;
    void EnableClipingPlane(uint32) override;
    void DisableCliping(uint32) override;
    void EnableCulling() override;
    void DisableCulling() override;
    void SetViewPort(uint32, uint32, uint32, uint32) override;
    void BindTexture(uint32) const override;
    void BindImageTexture(uint32, GraphicsApi::TextureAccess) override;
    GraphicsApi::ID CreateShadowMap(uint32, uint32) override;

    void PolygonModeRender() override;
    void LineModeRender() override;
    void SetBlendFunction(GraphicsApi::BlendFunctionType) override;

    void UpdateMatrixes(uint32, const std::vector<mat4>&) override;
    void UpdateMesh(uint32, const GraphicsApi::MeshRawData&, const std::set<VertexBufferObjects>&) override;
    void UpdateLineMesh(uint32, const GraphicsApi::LineMesh&) override;
    void UpdateOffset(uint32, const std::vector<vec4>&) override;
    void UpdateBlend(uint32, const std::vector<float>&) override;

    void LoadViewMatrix(const mat4&) override;
    void LoadProjectionMatrix(const mat4&) override;
    void DrawLine(const vec3& color, const vec3& from, const vec3& to) override;
    mat4 PrepareMatrixToLoad(const mat4& m) override;
    std::vector<uint8> GetTextureData(uint32) const override;
    const GraphicsApi::TextureInfo& GetTextureInfo(uint32) const override;
    void TakeSnapshoot(const std::filesystem::path&) const override;

    IFrameBuffer& GetDefaultFrameBuffer() override;
    IFrameBuffer& CreateFrameBuffer(const std::vector<GraphicsApi::FrameBuffer::Attachment>&) override;
    void DeleteFrameBuffer(IFrameBuffer&) override;

    std::optional<Utils::Image> GetImage(IdType) const override;

private:
    void CreateDebugNormalMesh(uint32, const GraphicsApi::MeshRawData&);
    void DeleteMesh(uint32);
    void DeleteDebugNormalMesh(uint32);
    void DeleteShader(uint32);
    void ClearRest();
    void allocatedBytes(int64);
    void ApplyTextureParameters(GLenum, GraphicsApi::TextureFilter, GraphicsApi::TextureMipmap);

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;

    std::unique_ptr<GraphicsApi::IWindowApi> windowApi_;
    Color bgColor_;
    uint32 bindedShaderBuffers_[MAX_SHADER_BUFFER_SIZE];

    std::unordered_map<uint32, OpenGLMesh> openGlMeshes_;
    SimpleQuad quad_;
    SimpleQuad quadTs_;
    bool polygonMode_;
    bool useLowGLversion_;
    vec2i openGLVersion_;

    int64 allocatedBytes_{0};
};
}  // namespace OpenGLApi
