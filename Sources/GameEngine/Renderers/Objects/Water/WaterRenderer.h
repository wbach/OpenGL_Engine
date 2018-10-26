#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "Shaders/WaterShader.h"

namespace GameEngine
{
class WaterRenderer : public IRenderer
{
public:
    WaterRenderer(RendererContext& context);
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;
    void Render(Scene* scene);

private:
    RendererContext& context_;
    WaterShader shader;
};
}  // GameEngine
