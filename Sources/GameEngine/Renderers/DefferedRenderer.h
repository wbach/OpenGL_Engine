#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include "GameEngine/Api/IGraphicsApi.h"
#include "IRenderer.h"
#include "Postproccesing/PostprocessingRenderersManager.h"
#include "RendererContext.h"

namespace GameEngine
{
class Projection;
typedef std::unique_ptr<IRenderer> RendererPtr;
typedef std::vector<RendererPtr> RendererVecPtr;

class DefferedRenderer : public IRenderer
{
public:
    DefferedRenderer(IGraphicsApiPtr graphicsApi, Projection* projection_matrix, IShaderFactory& shaderFactory,
                 std::function<void(RendererFunctionType, RendererFunction)> rendererFunction);
    ~DefferedRenderer();
    // Loading lights itp to shader
    virtual void Init() override;
    virtual void Subscribe(GameObject* gameObject) override;
    virtual void UnSubscribe(GameObject* gameObject) override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;

private:
    void CreateRenderers();
    void Prepare(Scene*);
    void OnEndFrame(Scene*);
    template <class T>
    void AddRenderer();

private:
    RendererContext context_;
    RendererVecPtr renderers;
    PostProcessingManager postprocessingRenderersManager_;
};
}  // GameEngine
