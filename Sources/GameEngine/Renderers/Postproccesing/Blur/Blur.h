#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "Shaders/BlurShader.h"

namespace GameEngine
{
class Blur : public PostprocessingRenderer
{
public:
    virtual void Init() override;
    virtual void Prepare() override;
    virtual void Render(const Scene&) override;
    virtual void ReloadShaders() override;

private:
    std::unique_ptr<BlurShader> shader_;
};
}  // namespace GameEngine
