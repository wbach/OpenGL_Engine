#include "SSAO.h"

#include <GraphicsApi/IGraphicsApi.h>
#include "GameEngine/Renderers/RendererContext.h"

namespace GameEngine
{
SSAORenderer::SSAORenderer(RendererContext& context)
    : PostprocessingRenderer(context)
    , ssaoShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::SSAO)
    , blurRenderer_(context)
{
}
void SSAORenderer::Init()
{
    blurRenderer_.Init();

    ssaoShader_.Init();
    ssaoShader_.Start();
    GenKernel();
    // ssaoShader_->Load(SSAOShaderUniforms::SampleRadius, 1.5f);
    // ssaoShader_->Load(SSAOShaderUniforms::ProjectionMatrix, rendererContext_->projection_.GetProjectionMatrix());
    ssaoShader_.Stop();
}
void SSAORenderer::CleanUp()
{
}
void SSAORenderer::Prepare()
{
}
void SSAORenderer::Render(const Scene&)
{
    SSAOPass();
    BlurPass();
}
void SSAORenderer::ReloadShaders()
{
    ssaoShader_.Reload();
}
void SSAORenderer::SSAOPass()
{
    //    auto positionTexture = rendererContext_.defferedFrameBuffer_.GetTexture(0);

    ssaoShader_.Start();
    // rendererContext_.graphicsApi_.ActiveTexture(0, positionTexture);
    rendererContext_.graphicsApi_.RenderQuad();
    ssaoShader_.Stop();
}
void SSAORenderer::BlurPass()
{
    // blurRenderer_.Render();
}
void SSAORenderer::GenKernel()
{
    std::vector<vec3> kernel;
    kernel.resize(KERNEL_SIZE);

    for (uint32 i = 0; i < KERNEL_SIZE; i++)
    {
        float scale = (float)i / (float)(KERNEL_SIZE);
        vec3 v;
        v.x = 2.0f * (float)rand() / static_cast<float>(RAND_MAX) - 1.0f;
        v.y = 2.0f * (float)rand() / static_cast<float>(RAND_MAX) - 1.0f;
        v.z = 2.0f * (float)rand() / static_cast<float>(RAND_MAX) - 1.0f;
        // Use an acceleration function so more points are
        // located closer to the origin
        v *= (0.1f + 0.9f * scale * scale);

        kernel[i] = v;
    }
    // ssaoShader_->Load(SSAOShaderUniforms::Kernel, kernel);
}
}  // namespace GameEngine
