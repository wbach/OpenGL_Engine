#include "Fxaa.h"

#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"

namespace GameEngine
{
struct FxaaBuffer
{
    vec4 fxaaVariables;
    AlignWrapper<vec2> invertedScreenSize;
};

Fxaa::Fxaa(RendererContext& context)
    : PostprocessingRenderer(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Fxaa)
{
}
void Fxaa::Init()
{
    shader_.Init();

    if (not fxaaBufferId_)
    {
        fxaaBufferId_ = rendererContext_.graphicsApi_.CreateShaderBuffer(6, sizeof(FxaaBuffer));
        if (fxaaBufferId_)
        {
            FxaaBuffer buffer;
            buffer.invertedScreenSize = vec2(1.f / rendererContext_.projection_.GetRenderingSize().x,
                                             1.f / rendererContext_.projection_.GetRenderingSize().y);
            float fxaaSpanMax         = 8.f;
            float fxaaReduceMin       = 1.f / 128.f;
            float fxaaReduceMul       = 0.f /*1.f / 8.f*/;
            buffer.fxaaVariables      = vec4(fxaaSpanMax, fxaaReduceMin, fxaaReduceMul, 0.f);
            rendererContext_.graphicsApi_.UpdateShaderBuffer(*fxaaBufferId_, &buffer);
        }
    }
}
void Fxaa::Prepare()
{
}
void Fxaa::Render(const Scene&)
{
    shader_.Start();
    rendererContext_.graphicsApi_.BindShaderBuffer(*fxaaBufferId_);
    rendererContext_.graphicsApi_.RenderQuad();
    shader_.Stop();
}
void Fxaa::ReloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
