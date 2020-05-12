#pragma once
#include "BaseRenderer.h"

namespace GameEngine
{
struct Time;
class DefferedRenderer : public BaseRenderer
{
public:
    DefferedRenderer(RendererContext& context);
    ~DefferedRenderer();

    virtual void Init() override;
    virtual void ReloadShaders() override;

private:
    void CreateRenderers();
    void Prepare(const Scene&, const Time&);
    void OnEndFrame(const Scene&, const Time&);

private:
    PostProcessingManager postprocessingRenderersManager_;
    bool resizeRenderingMode_;
};
}  // namespace GameEngine
