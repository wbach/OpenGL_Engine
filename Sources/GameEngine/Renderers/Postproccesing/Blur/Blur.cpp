#include "Blur.h"

namespace GameEngine
{
Blur::Blur(RendererContext& context, PostprocessFrameBuffer** postprocessFrameBuffer)
    : PostprocessingRenderer(context, postprocessFrameBuffer)
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
    (*postprocessFrameBuffer_)->BindTextures();
    rendererContext_.graphicsApi_.RenderQuad();
    shader_.Stop();
}
void Blur::ReloadShaders()
{
    shader_.Reload();
}
}  // namespace GameEngine
