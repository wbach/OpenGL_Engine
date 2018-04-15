#include "SSAO.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"

namespace GameEngine
{
	SSAORenderer::SSAORenderer(IGraphicsApiPtr graphicsApi, CProjection* projection, CFrameBuffer* frambuffer)
		: CRenderer(frambuffer)
		, graphicsApi_(graphicsApi)
		, projection_(projection)
		, shader_(graphicsApi)
	{
	}
	void SSAORenderer::Init()
	{
		shader_.Init();
	}
	void SSAORenderer::PrepareFrame(GameEngine::Scene * scene)
	{
	}
	void SSAORenderer::Render(GameEngine::Scene * scene)
	{
	}
	void SSAORenderer::EndFrame(GameEngine::Scene * scene)
	{
		auto positionTexture = target->GetTexture(0);

		shader_.Start();
		graphicsApi_->ActiveTexture(0, positionTexture);
		graphicsApi_->RenderQuad();
		shader_.Stop();
	}
	void SSAORenderer::ReloadShaders()
	{
		shader_.Stop();
		shader_.Reload();
		shader_.Init();
	}
}
