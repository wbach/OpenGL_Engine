#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class Mesh;
class Model;
struct RendererContext;

struct SkydomeSubscriber
{
    Model* model_{nullptr};
};

class SkydomRenderer : public IRenderer
{
public:
    SkydomRenderer(RendererContext&);
    void init() override;
    void subscribe(GameObject&) override;
    void reloadShaders() override;
    void unSubscribe(GameObject&) override;
    void unSubscribeAll() override;
    void render() override;

private:
    void UpdateBuffer(const vec3& cameraPos);

private:
    RendererContext& context_;
    ShaderProgram shader_;
    GraphicsApi::ID perObjectUpdateId_;
    PerObjectUpdate perObjectUpdate_;
    SkydomeSubscriber subscriber_;
};
}  // namespace GameEngine
