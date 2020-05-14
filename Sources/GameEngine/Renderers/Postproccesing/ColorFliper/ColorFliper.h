#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class ColorFliper : public PostprocessingRenderer
{
public:
    ColorFliper(RendererContext&);
    virtual void Init() override;
    virtual void Prepare() override;
    virtual void Render(const Scene&) override;
    virtual void ReloadShaders() override;

private:
    ShaderProgram shader_;
};
}  // namespace GameEngine
