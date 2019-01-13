#pragma once
#include "BaseRenderer.h"

namespace GameEngine
{

class DefferedRenderer : public BaseRenderer
{
public:
    DefferedRenderer(IGraphicsApi& graphicsApi, Projection& projection_matrix, IShaderFactory& shaderFactory,
                 std::function<void(RendererFunctionType, RendererFunction)> rendererFunction);
    ~DefferedRenderer();
    // Loading lights itp to shader
    virtual void Init() override;

    virtual void ReloadShaders() override;

private:
    void CreateRenderers();
    void Prepare(Scene*);
    void OnEndFrame(Scene*);

private:
    PostProcessingManager postprocessingRenderersManager_;
};
}  // GameEngine
