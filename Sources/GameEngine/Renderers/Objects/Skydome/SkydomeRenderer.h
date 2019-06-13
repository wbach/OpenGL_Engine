#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"

namespace GameEngine
{
struct Time;
class Mesh;
class Model;
class IShaderProgram;
struct RendererContext;

struct SkydomeSubscriber
{
    Model* model_{nullptr};
};

class SkydomRenderer : public IRenderer
{
public:
    SkydomRenderer(RendererContext& context);
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void ReloadShaders() override;

private:
    void Render(const Scene& scene, const Time&);
    void UpdateBuffer(const vec3& cameraPos);

private:
    RendererContext& context_;
    std::unique_ptr<IShaderProgram> shader_;
    GraphicsApi::ID perObjectUpdateId_;
    PerObjectUpdate perObjectUpdate_;
    SkydomeSubscriber subscriber_;
};
}  // namespace GameEngine
