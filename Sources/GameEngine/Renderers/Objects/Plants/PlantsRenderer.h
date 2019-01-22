#pragma once
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"

class CProjection;

namespace GameEngine
{
class IShaderProgram;

class PlantsRenderer : public IRenderer
{
public:
    PlantsRenderer(RendererContext& context);
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;
    void Render(Scene* scene);

private:
    RendererContext& context_;
    std::unique_ptr<IShaderProgram> shader_;
};
}  // GameEngine
