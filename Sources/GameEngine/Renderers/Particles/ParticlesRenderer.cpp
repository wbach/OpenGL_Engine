#include "ParticlesRenderer.h"

namespace GameEngine
{
	ParticlesRenderer::ParticlesRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection * projection_matrix, CFrameBuffer* framebuffer)
		: CRenderer(framebuffer)
		, shader(graphicsApi)
		, graphicsApi_(graphicsApi)
		, projectionMatrix(projection_matrix)
	{
	}
	void ParticlesRenderer::Init()
	{
	}
	void ParticlesRenderer::PrepareFrame(GameEngine::Scene * scene)
	{
	}
	void ParticlesRenderer::Render(GameEngine::Scene * scene)
	{
	}
	void ParticlesRenderer::EndFrame(GameEngine::Scene * scene)
	{
	}
	void ParticlesRenderer::Subscribe(CGameObject * gameObject)
	{
	}
	void ParticlesRenderer::UnSubscribe(CGameObject * gameObject)
	{
	}
	void ParticlesRenderer::UnSubscribeAll()
	{
	}
	void ParticlesRenderer::ReloadShaders()
	{
	}
} // GameEngine
