#pragma once
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"

namespace GameEngine
{
class Mesh;
class Model;
class Projection;
class IShaderFactory;
class IShaderProgram;

namespace Components
{
class GrassRendererComponent;
}  // namespace Components

class GrassRenderer : public IRenderer
{
public:
    GrassRenderer(RendererContext& context);
    ~GrassRenderer();
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
    void RenderMesh(const Mesh& mesh);
    void PrepareShader(Scene* scene);

private:
    RendererContext& context_;
    std::unique_ptr<IShaderProgram> shader_;

    std::vector<std::pair<uint32_t, Components::GrassRendererComponent*>> subscribes_;
    float viewDistance = 30.f;
};
}  // namespace GameEngine
