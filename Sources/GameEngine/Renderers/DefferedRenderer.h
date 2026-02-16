#pragma once
#include <optional>

#include "BaseRenderer.h"
#include "GameEngine/Renderers/Postproccesing/LightShafts/LightShaftOclusionRenderer.h"
#include "GraphicsApi/IFrameBuffer.h"
#include "SkyPassRenderer/SkyPassRenderer.h"
#include "GameEngine/Renderers/Postproccesing/SSAO/SSAORenderer.h"
#include "Types.h"

namespace GameEngine
{
struct Time;
class DefferedRenderer : public BaseRenderer
{
public:
    DefferedRenderer(RendererContext&);
    ~DefferedRenderer() override;

    void init() override;
    void render() override;
    void reloadShaders() override;
    void setViewPort() override;
    void cleanUp() override;

private:
    void bindDefferedFbo();
    void unbindDefferedFbo();
    void createFrameBuffer();
    void createOrUpdateDefferedFrameBufferIfNeeded();

private:
    GraphicsApi::IFrameBuffer* defferedFrameBuffer_;
    std::optional<vec2ui> defferedFrameBufferSize_;
    PostProcessingManager postprocessingRenderersManager_;

    SSAORenderer ssaoRenderer;
    SkyPassRenderer skyPassRenderer;
    LightShaftOclusionRenderer lightShaftRenderer_;
    bool isReady_;
};
}  // namespace GameEngine
