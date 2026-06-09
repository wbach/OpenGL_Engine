
#pragma once
#include "GraphicsApi/IGraphicsApi.h"

namespace GraphicsApi::Dx11
{
class DirectXApi : public IGraphicsApi
{
public:
    DirectXApi();
    ~DirectXApi();
    void Init() override;
    void PrepareFrame() override;
    void EndFrame() override;
    void SetShadersFilesLocations(const std::filesystem::path&) override;
    void SetShaderQuaility(ShaderQuaility) override;
    void DebugNormalMeshGeneration(bool) override;
    void CreateContext() override;
    void DeleteContext() override;
    void PrintVersion() override;
    bool IsTesselationSupported() const override;
    std::vector<RendererType> GetSupportedRenderers() const override;
    IWindowApi& GetWindowApi() override;

    void SetDefaultTarget() override;
    void SetBackgroundColor(const Color&) override;
    const Color& GetBackgroundColor() const override;
    void EnableDepthTest() override;
    void DisableDepthTest() override;

    ID CreateShader(ShaderProgramType) override;
    void UseShader(uint32) override;

    ID CreateShaderBuffer(uint32 bindLocation, uint32 size, DrawFlag) override;
    ID CreateShaderStorageBuffer(uint32 bindLocation, uint32 size, DrawFlag) override;
    void UpdateShaderBuffer(uint32 id, void const* buffer) override;
    void UpdateShaderStorageBuffer(uint32 id, void const* buffer, uint32 size) override;
    void* MapShaderStorageBuffer(uint32 id, uint32 size, uint32 flags) override;
    void UnmapShaderStorageBuffer(uint32 id) override;

    uint32 BindShaderBuffer(uint32) override;

    ID CreateTexture(const Utils::Image&, TextureFilter, TextureMipmap) override;
    ID CreateTexture(const std::vector<Utils::Image>&, TextureFilter, TextureMipmap) override;
    std::optional<uint32> CreateTextureStorage(TextureType, TextureFilter, int32) override;
    ID CreateCubMapTexture(const std::array<Utils::Image, 6>&) override;
    void GenerateMipmaps(IdType) override;
    void CopyTexture(IdType src, IdType dst, uint32 width, uint32 height) override;
    void UpdateTexture(uint32, const vec2ui&, const Utils::Image&) override;
    void UpdateTexture(uint32, const Utils::Image&) override;
    std::vector<Utils::Image> GetImageArray(IdType) const override;
    void ClearTexture(uint32, const Color&) override;

    void EnableBlend() override;
    void DisableBlend() override;

    void EnableDepthMask() override;
    void DisableDepthMask() override;
    void ActiveTexture(uint32) override;
    void ActiveTexture(uint32, uint32) override;

    void DeleteObject(uint32) override;
    void DeleteObject(const std::vector<uint32>&) override;
    void DeleteShaderBuffer(uint32) override;
    void DeleteShaderBuffer(const std::vector<uint32>&) override;

    std::string GetBufferStatus() override;
    ID CreatePatchMesh(const std::vector<float>&) override;
    ID CreatePurePatchMeshInstanced(uint32, uint32) override;
    ID CreateMesh(const MeshRawData&, RenderType rendertype) override;
    ID CreateDynamicLineMesh() override;
    ID CreateParticle() override;
    ID CreateAnimatedParticle() override;
    void Compute(uint32, uint32, uint32) override;
    void RenderPurePatchedMeshInstances(uint32) override;
    void RenderMesh(uint32) override;
    void RenderProcedural(uint32) override;
    void RenderDebugNormals(uint32) override;
    void RenderTriangleStripMesh(uint32) override;
    void RenderMeshInstanced(uint32, uint32) override;
    void RenderPoints(uint32) override;
    void RenderQuad() override;
    void RenderQuadTs() override;
    void EnableClipingPlane(uint32) override;
    void DisableCliping(uint32) override;
    void EnableCulling() override;
    void DisableCulling() override;
    void SetViewPort(uint32, uint32, uint32, uint32) override;
    void BindTexture(uint32) const override;
    void BindImageTexture(uint32, TextureAccess) override;
    ID CreateShadowMap(uint32, uint32) override;

    void PolygonModeRender() override;
    void LineModeRender() override;
    void SetBlendFunction(BlendFunctionType) override;

    void UpdateMatrixes(uint32, const std::vector<mat4>&) override;
    void UpdateMesh(uint32, const MeshRawData&, const std::set<VertexBufferObjects>&) override;
    void UpdateLineMesh(uint32, const LineMesh&) override;
    void UpdateOffset(uint32, const std::vector<vec4>&) override;
    void UpdateBlend(uint32, const std::vector<float>&) override;

    void LoadViewMatrix(const mat4&) override;
    void LoadProjectionMatrix(const mat4&) override;
    void DrawLine(const vec3& color, const vec3& from, const vec3& to) override;
    mat4 PrepareMatrixToLoad(const mat4& m) override;
    std::vector<uint8> GetTextureData(uint32) const override;
    const TextureInfo& GetTextureInfo(uint32) const override;
    void TakeSnapshoot(const std::filesystem::path&) const override;
    std::optional<Utils::Image> GetImage(IdType) const override;

    IFrameBuffer& GetDefaultFrameBuffer() override;
    IFrameBuffer& CreateFrameBuffer(const std::vector<FrameBuffer::Attachment>&) override;
    void DeleteFrameBuffer(IFrameBuffer&) override;

private:
    void InitRenderTarget();
    void InitDepthSetncilView();
    void SetRasterState();
    void SetRenderTargets();

private:
    class Pimpl;
    std::unique_ptr<Pimpl> impl_;
    std::unique_ptr<IWindowApi> windowApi_;

private:
    float bgColor_[4];
    Color bgColor;
    std::filesystem::path shadersFileLocation_;
};
}  // namespace GraphicsApi::Dx11
