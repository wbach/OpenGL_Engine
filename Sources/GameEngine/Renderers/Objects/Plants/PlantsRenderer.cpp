#include "PlantsRenderer.h"

namespace GameEngine
{
PlantsRenderer::PlantsRenderer(RendererContext& context)
    : context_(context)
    , shader(context.graphicsApi_)
{
}
void PlantsRenderer::Init()
{
}
void PlantsRenderer::Render(Scene* scene)
{
}
void PlantsRenderer::Subscribe(GameObject* gameObject)
{
}
void PlantsRenderer::UnSubscribe(GameObject* gameObject)
{
}
void PlantsRenderer::UnSubscribeAll()
{
}
void PlantsRenderer::ReloadShaders()
{
}
}  // GameEngine
