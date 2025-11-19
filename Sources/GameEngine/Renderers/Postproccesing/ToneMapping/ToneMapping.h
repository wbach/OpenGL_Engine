#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class ToneMapping : public PostprocessingRenderer
{
public:
    ToneMapping(RendererContext&);
    void Init() override;
    void Prepare() override;
    void Render(const Scene&) override;
    void ReloadShaders() override;

private:
    ShaderProgram shader_;
    GraphicsApi::ID ToneMappingBufferId_;
};
}  // namespace GameEngine
