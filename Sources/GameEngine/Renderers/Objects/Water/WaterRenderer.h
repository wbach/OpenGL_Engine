#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GraphicsApi/IGraphicsApi.h"
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

private:
    void Render(Scene* scene, const Time&);

private:
    RendererContext& context_;
    WaterShader shader;
};
}  // namespace GameEngine
