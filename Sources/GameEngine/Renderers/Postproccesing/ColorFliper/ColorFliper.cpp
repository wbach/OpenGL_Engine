#include "ColorFliper.h"

namespace GameEngine
{
ColorFliper::ColorFliper(RendererContext& context, PostprocessFrameBuffer** postprocessFrameBuffer)
    : PostprocessingRenderer(context, postprocessFrameBuffer)
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
    (*postprocessFrameBuffer_)->BindTextures();
    rendererContext_.graphicsApi_.RenderQuad();
    shader_.Stop();
}
void ColorFliper::ReloadShaders()
{
    shader_.Reload();
}
}  // GameEngine
