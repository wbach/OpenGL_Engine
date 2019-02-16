#pragma once
#include "GameEngine/Renderers/Postproccesing/Blur/Blur.h"
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "Shaders/SSAOShader.h"

namespace GameEngine
{
const uint32 KERNEL_SIZE = 128;
class SSAORenderer : public PostprocessingRenderer
{
public:
    virtual void Init() override;
    virtual void Prepare() override;
    virtual void Render(const Scene&) override;
    virtual void ReloadShaders() override;

private:
    void GenKernel();
    void SSAOPass();
    void BlurPass();

private:
    std::unique_ptr<SSAOShader> ssaoShader_;
    Blur blurRenderer_;
};
}  // namespace GameEngine
