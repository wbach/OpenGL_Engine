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
	void WaterRenderer::Render(Scene * scene)
	{
	}
	void WaterRenderer::Subscribe(CGameObject * gameObject)
	{
	}
	void WaterRenderer::UnSubscribe(CGameObject * gameObject)
	{
	}
	void WaterRenderer::UnSubscribeAll()
	{
	}
	void WaterRenderer::ReloadShaders()
	{
	}
} // GameEngine
