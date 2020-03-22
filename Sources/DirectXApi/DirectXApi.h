
#pragma once
#include "GraphicsApi/IGraphicsApi.h"

namespace DirectX
{
class DirectXApi : public GraphicsApi::IGraphicsApi
{
public:
    DirectXApi();
    ~DirectXApi();
    virtual void Init() override;
    virtual void SetShadersFilesLocations(const std::string& path) override;
    virtual void SetShaderQuaility(GraphicsApi::ShaderQuaility) override;
    virtual void CreateContext() override;
    virtual void DeleteContext() override;
    virtual void PrintVersion() override;
    virtual bool IsTesselationSupported() const override;
    virtual GraphicsApi::IWindowApi& GetWindowApi() override;

    virtual void PrepareFrame() override;
    virtual void SetDefaultTarget() override;
    virtual void SetBackgroundColor(const vec3&) override;
    virtual void EnableDepthTest() override;
    virtual void DisableDepthTest() override;

    virtual GraphicsApi::ID CreateShader(GraphicsApi::ShaderProgramType) override;
    virtual void UseShader(uint32) override;

    virtual GraphicsApi::ID CreateShaderBuffer(uint32 bindLocation, uint32 size) override;
    virtual void UpdateShaderBuffer(uint32 id, void const* buffer) override;
    virtual uint32 BindShaderBuffer(uint32) override;

    virtual GraphicsApi::ID CreateTexture(GraphicsApi::TextureType, GraphicsApi::TextureFilter,
                                          GraphicsApi::TextureMipmap,
                                 GraphicsApi::BufferAtachment, vec2ui, void* data) override;
    virtual std::optional<uint32> CreateTextureStorage(GraphicsApi::TextureType, GraphicsApi::TextureFilter,
                                                       int32) override;
    virtual GraphicsApi::ID CreateCubMapTexture(vec2ui, std::vector<void*>) override;

    virtual void UpdateTexture(uint32, const vec2ui&, const vec2ui&, void* data) override;
    virtual void UpdateTexture(uint32, const vec2ui&, void* data) override;

    virtual void SetBuffers(const std::vector<GraphicsApi::BufferAtachment>&) override;
    virtual void ClearBuffer(GraphicsApi::BufferType) override;
    virtual void ClearBuffers(const std::vector<GraphicsApi::BufferType>&) override;

    virtual void EnableBlend() override;
    virtual void DisableBlend() override;

    virtual void EnableDepthMask() override;
    virtual void DisableDepthMask() override;
    virtual void ActiveTexture(uint32) override;
    virtual void ActiveTexture(uint32, uint32) override;

    virtual GraphicsApi::ID CreateBuffer() override;
    virtual void BindBuffer(GraphicsApi::BindType, uint32) override;

    virtual void DeleteObject(uint32) override;
    virtual void DeleteShaderBuffer(uint32) override;

    virtual std::string GetBufferStatus() override;
    virtual GraphicsApi::ID CreatePatchMesh(const std::vector<float>&) override;
    virtual GraphicsApi::ID CreatePurePatchMeshInstanced(uint32, uint32) override;
    virtual GraphicsApi::ID CreateMesh(const GraphicsApi::MeshRawData&, GraphicsApi::RenderType rendertype) override;
    virtual GraphicsApi::ID CreateParticle() override;
    virtual GraphicsApi::ID CreateAnimatedParticle() override;
    virtual void Compute(uint32, uint32, uint32) override;
    virtual void RenderPurePatchedMeshInstances(uint32) override;
    virtual void RenderMesh(uint32) override;
    virtual void RenderTriangleStripMesh(uint32) override;
    virtual void RenderMeshInstanced(uint32, uint32) override;
    virtual void RenderPoints(uint32) override;
    virtual void RenderQuad() override;
    virtual void RenderQuadTs() override;
    virtual void EnableCulling() override;
    virtual void DisableCulling() override;
    virtual void SetViewPort(uint32, uint32, uint32, uint32) override;
    virtual void BindTexture(uint32) const override;
    virtual void BindImageTexture(uint32, GraphicsApi::TextureAccess) override;
    virtual GraphicsApi::ID CreateShadowMap(uint32, uint32) override;

    virtual void PolygonModeRender() override;
    virtual void LineModeRender() override;
    virtual void SetBlendFunction(GraphicsApi::BlendFunctionType) override;

    virtual void UpdateMatrixes(uint32, const std::vector<mat4>&) override;
    virtual void UpdateOffset(uint32, const std::vector<vec4>&) override;
    virtual void UpdateBlend(uint32, const std::vector<float>&) override;

    virtual GraphicsApi::ID CloneImage(uint32) override;

    // temp
    virtual void CreateFont(const std::string&) override;
    virtual void PrintText(const std::string&, const vec2i&) override;

    virtual void LoadViewMatrix(const mat4&) override;
    virtual void LoadProjectionMatrix(const mat4&) override;
    virtual void DrawLine(const vec3& color, const vec3& from, const vec3& to) override;
    virtual mat4 PrepareMatrixToLoad(const mat4& m) override;
    virtual std::vector<uint8> GetTextureData(uint32) const override;
    virtual const GraphicsApi::TextureInfo& GetTextureInfo(uint32) const override;
    virtual void TakeSnapshoot(const std::string& path) const override;

private:
    void InitRenderTarget();
    void InitDepthSetncilView();
    void SetRasterState();
    void SetRenderTargets();
    void UpdateDepthStencilState();

private:
    class Pimpl;
    std::unique_ptr<Pimpl> impl_;
    std::unique_ptr<GraphicsApi::IWindowApi> windowApi_;

private:
    float bgColor_[4];
    std::string shadersFileLocation_;
};
}  // namespace DirectX
