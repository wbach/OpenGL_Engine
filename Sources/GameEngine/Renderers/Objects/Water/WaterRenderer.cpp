#include "WaterRenderer.h"

namespace GameEngine
{
	WaterRenderer::WaterRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection * projection_matrix, CFrameBuffer* framebuffer)
		: CRenderer(framebuffer)
		, graphicsApi_(graphicsApi)
		, shader(graphicsApi)
		, projectionMatrix(projection_matrix)
	{
	}
	void WaterRenderer::Init()
	{
	}
	void WaterRenderer::PrepareFrame(GameEngine::Scene * scene)
	{
	}
	void WaterRenderer::Render(GameEngine::Scene * scene)
	{
	}
	void WaterRenderer::EndFrame(GameEngine::Scene * scene)
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
