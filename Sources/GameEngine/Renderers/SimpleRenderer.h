#pragma once
#include "GameEngine/Renderers/RendererFunctionType.h"
#include "IRenderer.h"
#include "Objects/Entity/Shaders/SimpleEnityShader.h"

class CModel;
struct SMaterial;
class CProjection;

namespace GameEngine
{
class Entity;

class SimpleRenderer : public IRenderer
{
public:
    SimpleRenderer(IGraphicsApiPtr, CProjection* projection_matrix,
                   std::function<void(RendererFunctionType, RendererFunction)> rendererFunction);

    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void ReloadShaders() override;
    void RenderModel(CModel* model, const mat4& transform_matrix) const;

private:
    void PrepareFrame(Scene* scene);
    void Render(Scene* scene);
    void EndFrame(Scene* scene);
    void BindTextures(const SMaterial& material) const;

private:
    IGraphicsApiPtr graphicsApi_;
    CSimpleEntityShader shader;
    CProjection* projectionMatrix;

    std::list<Entity*> subscribes;
};
}  // GameEngine
