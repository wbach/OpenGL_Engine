#pragma once
#include "GameEngine/Renderers/RendererFunctionType.h"
#include "IRenderer.h"
#include "Objects/Entity/Shaders/SimpleEnityShader.h"

namespace GameEngine
{
class Model;
struct Material;
class Projection;
class Entity;

class SimpleRenderer : public IRenderer
{
public:
    SimpleRenderer(IGraphicsApiPtr, Projection* projection_matrix,
                   std::function<void(RendererFunctionType, RendererFunction)> rendererFunction);

    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void ReloadShaders() override;
    void RenderModel(Model *model, const mat4& transform_matrix) const;

private:
    void PrepareFrame(Scene* scene);
    void Render(Scene* scene);
    void EndFrame(Scene* scene);
    void BindTextures(const Material& material) const;

private:
    IGraphicsApiPtr graphicsApi_;
    SimpleEntityShader shader;
    Projection* projectionMatrix;

    std::list<Entity*> subscribes;
};
}  // GameEngine
