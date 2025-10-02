#include "ColorFliper.h"

#include <GraphicsApi/IGraphicsApi.h>

#include "GameEngine/Renderers/RendererContext.h"

namespace GameEngine
{
ColorFliper::ColorFliper(RendererContext& context)
    : PostprocessingRenderer(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::ColorFilper)
{
}
void ColorFliper::Init()
{
    shader_.Init();
}
void ColorFliper::Prepare()
{
}
void ColorFliper::Render(const Scene&)
{
    shader_.Start();
    rendererContext_.graphicsApi_.RenderQuad();
    shader_.Stop();
}
void ColorFliper::ReloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
