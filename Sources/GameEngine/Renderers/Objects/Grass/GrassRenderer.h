#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GrassShaderBuffer.h"

namespace GameEngine
{
struct Time;
class Mesh;
class Model;
class Projection;

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
    virtual void UnSubscribe(GameObject* gameObject);
    virtual void UnSubscribeAll();
    virtual void ReloadShaders() override;

private:
    void Render(const Scene& scene, const Time& threadTime);

private:
    void InitShaderBuffer();
    void PrepareRender(const Scene& scene);
    void EndRender() const;
    void RenderSubscribes();
    void RenderModel(const Model& model);
    void RenderMesh(const Mesh& mesh);
    void PrepareShader(const Scene& scene);

private:
    RendererContext& context_;
    ShaderProgram shader_;

    GrassShaderBuffer grassShaderBuffer_;
    GraphicsApi::ID grassShaderBufferId_;

    std::vector<std::pair<uint32_t, Components::GrassRendererComponent*>> subscribes_;
    float viewDistance = 30.f;
};
}  // namespace GameEngine
