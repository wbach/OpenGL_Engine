#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "GameEngine/Components/IComponent.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "IRenderer.h"
#include "Postproccesing/PostprocessingRenderersManager.h"
#include "RendererContext.h"

namespace GameEngine
{
class Projection;
typedef std::unique_ptr<IRenderer> RendererPtr;
typedef std::vector<RendererPtr> Renderers;

class BaseRenderer : public IRenderer
{
public:
    BaseRenderer(RendererContext&);
    ~BaseRenderer();

    void init() override;
    void prepare() override;
    void render() override;
    virtual void setViewPort();
    void blendRender() override;
    void subscribe(GameObject&) override;
    void unSubscribe(GameObject&) override;
    void unSubscribe(const Components::IComponent&) override;
    void unSubscribeAll() override;
    void reloadShaders() override;

protected:
    void initRenderers();
    void createRenderers();

    template <class T>
    void addRenderer()
    {
        renderers.push_back(std::make_unique<T>(context_));
    }

protected:
    RendererContext& context_;
    Renderers renderers;
};
}  // namespace GameEngine
