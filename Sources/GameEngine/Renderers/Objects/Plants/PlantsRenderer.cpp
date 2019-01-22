#include "PlantsRenderer.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GraphicsApi/ShadersTypes.h"
#include "GameEngine/Shaders/IShaderProgram.h"

namespace GameEngine
{
PlantsRenderer::PlantsRenderer(RendererContext& context)
    : context_(context)
{
    shader_ = context.shaderFactory_.create(GraphicsApi::Shaders::Plants);
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
