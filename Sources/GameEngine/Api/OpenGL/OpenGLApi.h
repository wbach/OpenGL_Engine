#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Resources/SimpleModels/Quad.h"
#include "OpenGLMesh.h"
#include "OpenGLShaderProgram.h"
#include "optional.hpp"

namespace GameEngine
{
class OpenGLApi : public IGraphicsApi
{
public:
    OpenGLApi();
    OpenGLApi(IWindowApiPtr windowApi);
    virtual void Init() override;
    virtual void SetShaderQuaility(ShaderQuaility) override;
    virtual void CreateContext() override;
    virtual void DeleteContext() override;
    virtual void PrintVersion() override;
    virtual IWindowApiPtr GetWindowApi() override;

    virtual void PrepareFrame() override;
    virtual void SetDefaultTarget() override;
    virtual void SetBackgroundColor(const vec3&) override;
    virtual void EnableDepthTest() override;
    virtual void DisableDepthTest() override;

    virtual uint32 CreateShader(const ShadersFiles&, GraphicsApiFunctions) override;
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

    virtual uint32 CreateTexture(TextureType, TextureFilter, TextureMipmap, BufferAtachment, vec2ui,
                                 void* data) override;
    virtual uint32 CreateCubMapTexture(vec2ui, std::vector<void*>) override;

    virtual void SetBuffers(const std::vector<BufferAtachment>&) override;
    virtual void ClearBuffer(BufferType) override;
    virtual void ClearBuffers(const std::vector<BufferType>&) override;

    virtual void EnableBlend() override;
    virtual void DisableBlend() override;

    virtual void EnableDepthMask() override;
    virtual void DisableDepthMask() override;
    virtual void ActiveTexture(uint32) override;
    virtual void ActiveTexture(uint32, uint32) override;

    virtual uint32 CreateBuffer() override;
    virtual void BindBuffer(BindType, uint32) override;

    virtual void DeleteObject(uint32) override;

    virtual std::string GetBufferStatus() override;
    virtual uint32 CreatePurePatchMeshInstanced(uint32, uint32) override;
    virtual uint32 CreateMesh(const MeshRawData&) override;
    virtual uint32 CreateParticle() override;
    virtual uint32 CreateAnimatedParticle() override;
    virtual void RenderPurePatchedMeshInstances(uint32) override;
    virtual void RenderMesh(uint32) override;
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
    virtual void SetBlendFunction(BlendFunctionType) override;

    virtual void UpdateMatrixes(uint32, const std::vector<mat4>&) override;
    virtual void UpdateOffset(uint32, const std::vector<vec4>&) override;
    virtual void UpdateBlend(uint32, const std::vector<float>&) override;

    virtual uint32 CloneImage(uint32) override;

    // temp
    virtual void CreateFont(const std::string&) override;
    virtual void PrintText(const std::string&, const vec2i&) override;
    virtual ShadersFiles GetShaderFiles(Shaders shaderType) override;

private:
    ShadersFiles GetFullDefferedShaderFiles(Shaders shaderType);
    ShadersFiles GetSimpleForwardShaderFiles(Shaders shaderType);

    void DeleteMesh(uint32);
    uint32 ConvertAndRememberId(uint32 id);
    void DeleteShader(uint32);

    bool AddShader(uint32 id, const std::string& filename, GameEngine::ShaderType mode);
    bool FinalizeShader(uint32 programId, GraphicsApiFunctions);

private:
    ShaderQuaility shaderQuality_;
    uint32 activeBuffer_;
    uint32 objectId_;
    IWindowApiPtr windowApi_;
    uint32 usedShader;
    vec3 bgColor_;
    std::unordered_map<ShaderType, uint32> shaderTypeMap_;
    std::unordered_map<TextureFilter, uint32> textureFilterMap_;
    std::unordered_map<BufferAtachment, uint32> bufferAtachmentMap_;
    std::unordered_map<BufferType, uint32> bufferTypeMap_;
    std::unordered_map<uint32, OpenGLShaderProgram> shaderPrograms_;
    std::unordered_map<uint32, OpenGLMesh> openGlMeshes_;
    std::unordered_map<uint32, uint32> idToGlId_;
    SimpleQuad quad_;
    SimpleQuad quadTs_;
    bool polygonMode;
};
}  // GameEngine
