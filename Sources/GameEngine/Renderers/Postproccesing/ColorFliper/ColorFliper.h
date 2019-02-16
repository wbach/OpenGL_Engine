#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "Shaders/ColorFliperShader.h"

namespace GameEngine
{
class ColorFliper : public PostprocessingRenderer
{
public:
    virtual void Init() override;
    virtual void Prepare() override;
    virtual void Render(const Scene&) override;
    virtual void ReloadShaders() override;

private:
    std::unique_ptr<ColorFliperShader> shader_;
};
}  // GameEngine
