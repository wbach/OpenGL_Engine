#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
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
    GrassRenderer(RendererContext&);
    ~GrassRenderer();
    void init() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&);
    void unSubscribeAll() override;
    void reloadShaders() override;
    void render() override;

private:
    void InitShaderBuffer();
    void EndRender() const;
    void RenderSubscribes();
    void RenderModel(const Model& model);
    void RenderMesh(const Mesh& mesh);
    void prepareShader();

private:
    RendererContext& context_;
    ShaderProgram shader_;

    GrassShaderBuffer grassShaderBuffer_;
    GraphicsApi::ID grassShaderBufferId_;

    std::vector<std::pair<uint32_t, Components::GrassRendererComponent*>> subscribes_;
    float viewDistance = 30.f;
};
}  // namespace GameEngine
