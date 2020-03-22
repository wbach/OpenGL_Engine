#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "GraphicsApi/IGraphicsApi.h"
#include "IRenderer.h"
#include "Postproccesing/PostprocessingRenderersManager.h"
#include "RendererContext.h"

namespace GameEngine
{
class Projection;
typedef std::unique_ptr<IRenderer> RendererPtr;
typedef std::vector<RendererPtr> RendererVecPtr;

class BaseRenderer : public IRenderer
{
public:
    BaseRenderer(GraphicsApi::IGraphicsApi& graphicsApi, Projection& projection_matrix,
                 std::function<void(RendererFunctionType, RendererFunction)> rendererFunction);
    ~BaseRenderer();
    // Loading lights itp to shader
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;

protected:
    void InitRenderers();
    void CreateRenderers();
    void PreConfigure(const Scene&, const Time&);

    template <class T>
    void AddRenderer()
    {
        renderers.emplace_back(new T(context_));
    }

protected:
    std::unique_ptr<IFrameBuffer> defferedFrameBuffer_;
    std::unique_ptr<IShadowFrameBuffer> shadowsFrameBuffer_;
    RendererContext context_;
    RendererVecPtr renderers;
};
}  // namespace GameEngine
