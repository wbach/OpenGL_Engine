#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class Outline : public PostprocessingRenderer
{
public:
    Outline(RendererContext&);
    virtual void Init() override;
    virtual void Prepare() override;
    virtual void Render(const Scene&) override;
    virtual void ReloadShaders() override;

private:
    ShaderProgram outlineShader_;
    GraphicsApi::ID bufferId_;
};
}  // namespace GameEngine
