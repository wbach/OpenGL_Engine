#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class Outline : public PostprocessingRenderer
{
public:
    Outline(RendererContext&);
    void Init() override;
    void Prepare() override;
    void Render(const Scene&) override;
    void ReloadShaders() override;
    void CleanUp() override;

private:
    ShaderProgram outlineShader_;
    GraphicsApi::ID bufferId_;
};
}  // namespace GameEngine
