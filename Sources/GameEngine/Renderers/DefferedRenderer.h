#pragma once
#include "BaseRenderer.h"

namespace GameEngine
{
struct Time;
class DefferedRenderer : public BaseRenderer
{
public:
    DefferedRenderer(GraphicsApi::IGraphicsApi& graphicsApi, Projection& projection_matrix,
                     std::function<void(RendererFunctionType, RendererFunction)> rendererFunction);
    ~DefferedRenderer();

    virtual void Init() override;
    virtual void ReloadShaders() override;

private:
    void CreateRenderers();
    void Prepare(const Scene&, const Time&);
    void OnEndFrame(const Scene&, const Time&);

private:
    PostProcessingManager postprocessingRenderersManager_;
};
}  // namespace GameEngine
