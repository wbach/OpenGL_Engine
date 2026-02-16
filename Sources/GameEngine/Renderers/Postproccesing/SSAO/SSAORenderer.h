#pragma once
#include "GameEngine/Renderers/Postproccesing/Blur/Blur.h"
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GraphicsApi/GraphicsApiDef.h"

namespace GameEngine
{
class SSAORenderer
{
public:
    SSAORenderer(RendererContext&);
    void Init();
    void Render(uint32, uint32);
    void ReloadShaders();
    void CleanUp();

private:
    void GenKernel();
    void GenerateNoiseTexture();
    void CreateFbos();
    void SSAOPass(uint32, uint32);
    void BlurPass();
    void CreateUniformBuffer();

    GraphicsApi::IFrameBuffer* createFrameBuffer();

private:
    RendererContext& context;
    ShaderProgram ssaoShader_;
    ShaderProgram blurShader_;

    GraphicsApi::ID ubuffer;
    std::vector<glm::vec3> kernel;
    int kernelSize = 64;
    GraphicsApi::ID noiseTextureId;
    std::optional<vec2ui> frameBufferSize;

    GraphicsApi::IFrameBuffer* ssaFbo{nullptr};
    GraphicsApi::IFrameBuffer* ssaBlurFbo{nullptr};

    GraphicsApi::ID ssaColorTextureId;
};
}  // namespace GameEngine
