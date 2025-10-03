#pragma once
#include <optional>
#include "BaseRenderer.h"
#include "GraphicsApi/IFrameBuffer.h"
#include "Types.h"

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
    void setViewPort() override;

private:
    void bindDefferedFbo();
    void unbindDefferedFbo();
    void createFrameBuffer();
    void updateDefferedFrameBufferIfNeeded();

private:
    GraphicsApi::IFrameBuffer* defferedFrameBuffer_;
    std::optional<vec2ui> defferedFrameBufferSize_;
    PostProcessingManager postprocessingRenderersManager_;
    bool isReady_;
};
}  // namespace GameEngine
