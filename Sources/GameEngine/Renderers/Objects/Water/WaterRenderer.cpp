#include "WaterRenderer.h"

namespace GameEngine
{
WaterRenderer::WaterRenderer(RendererContext& context)
    : context_(context)
    , shader(context.graphicsApi_)
{
}
void WaterRenderer::Init()
{
}
void WaterRenderer::Render(Scene* scene, const Time&)
{
}
void WaterRenderer::Subscribe(GameObject* gameObject)
{
}
void WaterRenderer::UnSubscribe(GameObject* gameObject)
{
}
void WaterRenderer::UnSubscribeAll()
{
}
void WaterRenderer::ReloadShaders()
{
}
}  // GameEngine
