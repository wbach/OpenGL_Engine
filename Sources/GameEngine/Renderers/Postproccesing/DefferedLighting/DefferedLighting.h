#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "Shaders/DefferedShader.h"

namespace GameEngine
{
class DefferedLighting : public PostprocessingRenderer
{
public:
    virtual void Init() override;
    virtual void Prepare(Scene*) override;
    virtual void Render(Scene*) override;
    virtual void ReloadShaders() override;

private:
    std::unique_ptr<DefferedShader> shader_;
};
}  // GameEngine
