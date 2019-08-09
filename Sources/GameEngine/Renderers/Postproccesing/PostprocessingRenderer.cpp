#include "PostprocessingRenderer.h"

namespace GameEngine
{
PostprocessingRenderer::~PostprocessingRenderer()
{
}
void PostprocessingRenderer::SetRendererContext(RendererContext* rendererContext)
{
    rendererContext_ = rendererContext;
}
void PostprocessingRenderer::SetPostProcessFrameBuffer(PostprocessFrameBuffer** postprocessFrameBuffer)
{
    postprocessFrameBuffer_ = postprocessFrameBuffer;
}
}  // namespace GameEngine
