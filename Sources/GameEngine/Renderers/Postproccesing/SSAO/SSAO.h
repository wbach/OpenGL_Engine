#pragma once
#include "GameEngine/Renderers/Postproccesing/Blur/Blur.h"
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
const uint32 KERNEL_SIZE = 128;
class SSAORenderer : public PostprocessingRenderer
{
public:
    SSAORenderer(RendererContext& context, PostprocessFrameBuffer** postprocessFrameBuffer);
    virtual void Init() override;
    virtual void Prepare() override;
    virtual void Render(const Scene&) override;
    virtual void ReloadShaders() override;

private:
    void GenKernel();
    void SSAOPass();
    void BlurPass();

private:
    ShaderProgram ssaoShader_;
    Blur blurRenderer_;
};
}  // namespace GameEngine
