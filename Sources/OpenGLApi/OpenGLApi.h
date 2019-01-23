#pragma once
#include "GraphicsApi/IGraphicsApi.h"
#include "SimpleModels/Quad.h"
#include "OpenGLMesh.h"
#include "Shaders/ShaderManager.h"
#include "optional.hpp"

namespace OpenGLApi
{
class OpenGLApi : public GraphicsApi::IGraphicsApi
{
public:
    OpenGLApi();
    OpenGLApi(GraphicsApi::IWindowApiPtr windowApi);
    ~OpenGLApi();
    virtual void Init() override;
    virtual void SetShadersFilesLocations(const std::string& path) override;
    virtual void SetShaderQuaility(GraphicsApi::ShaderQuaility) override;
    virtual void CreateContext() override;
    virtual void DeleteContext() override;
    virtual void PrintVersion() override;
    virtual GraphicsApi::IWindowApiPtr GetWindowApi() override;

    virtual void PrepareFrame() override;
    virtual void SetDefaultTarget() override;
    virtual void SetBackgroundColor(const vec3&) override;
    virtual void EnableDepthTest() override;
    virtual void DisableDepthTest() override;

    virtual uint32 CreateShader(GraphicsApi::Shaders shaderType, GraphicsApi::GraphicsApiFunctions) override;
    virtual void UseShader(uint32) override;
    virtual uint32 GetShaderVariableLocation(uint32, const std::string&) override;

    virtual void BindAttribute(uint32, uint32, const std::string&) override;
    virtual void LoadValueToShader(uint32, int) override;
    virtual void LoadValueToShader(uint32, float) override;
    virtual void LoadValueToShader(uint32, uint32) override;
    virtual void LoadValueToShader(uint32, const vec2&) override;
    virtual void LoadValueToShader(uint32, const vec3&) override;
    virtual void LoadValueToShader(uint32, const vec4&) override;
    virtual void LoadValueToShader(uint32, const mat3&) override;
    virtual void LoadValueToShader(uint32, const mat4&) override;
    virtual void LoadValueToShader(uint32, const std::vector<float>&) override;
    virtual void LoadValueToShader(uint32, const std::vector<vec3>&) override;
    virtual void LoadValueToShader(uint32, const std::vector<mat4>&) override;

    virtual uint32 CreateTexture(GraphicsApi::TextureType, GraphicsApi::TextureFilter, GraphicsApi::TextureMipmap, GraphicsApi::BufferAtachment, vec2ui,
                                 void* data) override;
    virtual uint32 CreateCubMapTexture(vec2ui, std::vector<void*>) override;

    virtual void SetBuffers(const std::vector<GraphicsApi::BufferAtachment>&) override;
    virtual void ClearBuffer(GraphicsApi::BufferType) override;
    virtual void ClearBuffers(const std::vector<GraphicsApi::BufferType>&) override;

    virtual void EnableBlend() override;
    virtual void DisableBlend() override;

    virtual void EnableDepthMask() override;
    virtual void DisableDepthMask() override;
    virtual void ActiveTexture(uint32) override;
    virtual void ActiveTexture(uint32, uint32) override;

    virtual uint32 CreateBuffer() override;
    virtual void BindBuffer(GraphicsApi::BindType, uint32) override;

    virtual void DeleteObject(uint32) override;

    virtual std::string GetBufferStatus() override;
    virtual uint32 CreatePurePatchMeshInstanced(uint32, uint32) override;
    virtual uint32 CreateMesh(const GraphicsApi::MeshRawData&) override;
    virtual uint32 CreateParticle() override;
    virtual uint32 CreateAnimatedParticle() override;
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
    virtual void BindTexture(uint32) override;
    virtual uint32 CreateShadowMap(uint32, uint32) override;

    virtual void PolygonModeRender() override;
    virtual void LineModeRender() override;
    virtual void SetBlendFunction(GraphicsApi::BlendFunctionType) override;

    virtual void UpdateMatrixes(uint32, const std::vector<mat4>&) override;
    virtual void UpdateOffset(uint32, const std::vector<vec4>&) override;
    virtual void UpdateBlend(uint32, const std::vector<float>&) override;

    virtual uint32 CloneImage(uint32) override;

    // temp
    virtual void CreateFont(const std::string&) override;
    virtual void PrintText(const std::string&, const vec2i&) override;


    virtual void LoadViewMatrix(const mat4&) override;
    virtual void LoadProjectionMatrix(const mat4&) override;
    virtual void DrawLine(const vec3& color, const vec3& from, const vec3& to) override;

private:
    void DeleteMesh(uint32);
    void DeleteShader(uint32);

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> impl_;

    uint32 activeBuffer_;
    uint32 objectId_;
    GraphicsApi::IWindowApiPtr windowApi_;
    uint32 usedShader;
    vec3 bgColor_;
    std::unordered_map<GraphicsApi::ShaderType, uint32> shaderTypeMap_;
    std::unordered_map<GraphicsApi::TextureFilter, uint32> textureFilterMap_;
    std::unordered_map<GraphicsApi::BufferAtachment, uint32> bufferAtachmentMap_;
    std::unordered_map<GraphicsApi::BufferType, uint32> bufferTypeMap_;

    std::unordered_map<uint32, OpenGLMesh> openGlMeshes_;
    SimpleQuad quad_;
    SimpleQuad quadTs_;
    bool polygonMode_;
    bool useLowGLversion_;
    vec2i openGLVersion_;
};
}  // OpenGLApi
