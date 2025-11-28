#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class ToneMapping : public PostprocessingRenderer
{
public:
    ToneMapping(RendererContext&);
    ~ToneMapping() override;
    void Init() override;
    void Prepare() override;
    void Render(const Scene&) override;
    void ReloadShaders() override;
    void CleanUp() override;

private:
    void UpdateBuffer();

private:
    ShaderProgram shader_;
    GraphicsApi::ID toneMappingBufferId_;
    GraphicsApi::ID gammaSubId_;
    GraphicsApi::ID exposureSubId_;
};
}  // namespace GameEngine
