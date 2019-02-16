#pragma once
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/ResourceManager.h"
#include "GraphicsApi/IGraphicsApi.h"

namespace GameEngine
{
struct Time;
class Mesh;
class Model;
class IShaderProgram;
struct RendererContext;

struct SkydomeSubscriber
{
    Model* model_;
};

typedef std::vector<SkydomeSubscriber> SkyBoxSubscribes;

class SkydomRenderer : public IRenderer
{
public:
    SkydomRenderer(RendererContext& context);
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void ReloadShaders() override;

private:
    void Render(const Scene& scene, const Time&);

private:
    RendererContext& context_;
    std::unique_ptr<IShaderProgram> shader_;
    SkyBoxSubscribes subscribes_;
};
}  // namespace GameEngine
