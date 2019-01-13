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
    void LoadLights(Scene* scene);
    void PrepareApiStateToRender();
    void RetriveChanges();

private:
    std::unique_ptr<DefferedShader> shader_;
};
}  // GameEngine
