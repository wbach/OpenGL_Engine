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
    SSAORenderer(RendererContext&);
    void Init() override;
    void Prepare() override;
    void Render(const Scene&) override;
    void ReloadShaders() override;
    void CleanUp() override;

private:
    void GenKernel();
    void SSAOPass();
    void BlurPass();

private:
    ShaderProgram ssaoShader_;
    Blur blurRenderer_;
};
}  // namespace GameEngine
