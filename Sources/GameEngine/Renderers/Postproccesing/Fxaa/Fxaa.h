#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class Fxaa : public PostprocessingRenderer
{
public:
    Fxaa(RendererContext&);
    virtual void Init() override;
    virtual void Prepare() override;
    virtual void Render(const Scene&) override;
    virtual void ReloadShaders() override;

private:
    ShaderProgram shader_;
    GraphicsApi::ID fxaaBufferId_;
};
}  // namespace GameEngine
