#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "Shaders/GrassShader.h"

namespace GameEngine
{
class Mesh;
class Model;
class Projection;
struct Grass;

class GrassRenderer : public IRenderer
{
public:
    GrassRenderer(RendererContext& context);
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void ReloadShaders() override;
    void Render(Scene* scene);

private:
    void InitShader();
    void PrepareRender(Scene* scene);
    void EndRender() const;
    void RenderSubscribes();
    void RenderModel(Model* model);
    void RenderMesh(const Mesh &mesh);
    void PrepareShader(Scene* scene);

private:
    RendererContext& context_;
    GrassShader shader;

    std::list<Grass*> subscribes;
    float viewDistance = 30.f;
};
}  // GameEngine
