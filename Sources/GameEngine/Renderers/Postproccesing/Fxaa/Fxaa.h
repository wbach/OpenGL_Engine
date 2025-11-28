#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class Fxaa : public PostprocessingRenderer
{
public:
    Fxaa(RendererContext&);
    void Init() override;
    void Prepare() override;
    void Render(const Scene&) override;
    void ReloadShaders() override;
    void CleanUp() override;

private:
    ShaderProgram shader_;
    GraphicsApi::ID fxaaBufferId_;
};
}  // namespace GameEngine
