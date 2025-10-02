#include "Blur.h"

#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/RendererContext.h"

namespace GameEngine
{
Blur::Blur(RendererContext& context)
    : PostprocessingRenderer(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Blur)
{
}
void Blur::Init()
{
    shader_.Init();
}
void Blur::Prepare()
{
}
void Blur::Render(const Scene&)
{
    shader_.Start();
    rendererContext_.graphicsApi_.RenderQuad();
    shader_.Stop();
}
void Blur::ReloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
