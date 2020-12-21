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

    void init() override;
    void render() override;
    void reloadShaders() override;

private:
    void bindDefferedFbo();
    void unbindDefferedFbo();
    void createFrameBuffer();

private:
    GraphicsApi::IFrameBuffer* defferedFrameBuffer_;
    PostProcessingManager postprocessingRenderersManager_;
    bool resizeRenderingMode_;
    bool isReady_;
};
}  // namespace GameEngine
