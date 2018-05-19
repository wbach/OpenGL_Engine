#include "SSAO.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"

namespace GameEngine
{
	SSAORenderer::SSAORenderer(IGraphicsApiPtr graphicsApi, CProjection* projection, CFrameBuffer* frambuffer)
		: shader_(graphicsApi)
	{
	}
	void SSAORenderer::Init()
	{
		shader_.Init();
	}
	void SSAORenderer::Prepare(Scene * scene)
	{
	}
	void SSAORenderer::Render(Scene * scene)
	{
		auto positionTexture = rendererContext_->defferedFrameBuffer_->GetTexture(0);

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
