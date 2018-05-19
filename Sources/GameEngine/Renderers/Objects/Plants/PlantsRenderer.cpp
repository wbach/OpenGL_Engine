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
	void PlantsRenderer::Render(Scene * scene)
	{
	}
	void PlantsRenderer::Subscribe(CGameObject * gameObject)
	{
	}
	void PlantsRenderer::UnSubscribe(CGameObject * gameObject)
	{
	}
	void PlantsRenderer::UnSubscribeAll()
	{
	}
	void PlantsRenderer::ReloadShaders()
	{
	}
} // GameEngine
