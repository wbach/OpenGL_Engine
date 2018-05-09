#include "PlantsRenderer.h"

namespace GameEngine
{
	PlantsRenderer::PlantsRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection * projection_matrix, CFrameBuffer* framebuffer)
		: CRenderer(framebuffer)
		, graphicsApi_(graphicsApi)
		, shader(graphicsApi)
		, projectionMatrix(projection_matrix)
	{
	}
	void PlantsRenderer::Init()
	{
	}
	void PlantsRenderer::PrepareFrame(GameEngine::Scene * scene)
	{
	}
	void PlantsRenderer::Render(GameEngine::Scene * scene)
	{
	}
	void PlantsRenderer::EndFrame(GameEngine::Scene * scene)
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
