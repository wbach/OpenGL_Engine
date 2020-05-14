#pragma once
#include "BaseRenderer.h"
#include "GraphicsApi/IFrameBuffer.h"

namespace GameEngine
{
struct Time;
class DefferedRenderer : public BaseRenderer
{
public:
    DefferedRenderer(RendererContext& context);
    ~DefferedRenderer() override;

    void Init() override;
    void ReloadShaders() override;

private:
    void CreateRenderers();
    void Prepare(const Scene&, const Time&);
    void OnEndFrame(const Scene&, const Time&);
    void CreateFrameBuffer();

private:
    GraphicsApi::IFrameBuffer* defferedFrameBuffer_;
    PostProcessingManager postprocessingRenderersManager_;
    bool resizeRenderingMode_;
    bool isReady_;
};
}  // namespace GameEngine
