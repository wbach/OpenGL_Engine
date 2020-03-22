#include "PlantsRenderer.h"

namespace GameEngine
{
PlantsRenderer::PlantsRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Plants)
{

}
void PlantsRenderer::Init()
{
}
void PlantsRenderer::Render(const Scene&, const Time&)
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
