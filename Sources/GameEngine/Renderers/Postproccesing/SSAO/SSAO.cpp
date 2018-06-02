#include "SSAO.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"

namespace GameEngine
{
	void SSAORenderer::Init()
	{
		ssaoShader_.Init();
	}
	void SSAORenderer::Prepare(Scene * scene)
	{
	}
	void SSAORenderer::Render(Scene * scene)
	{
		SSAOPass();
		BlurPass();
	}
	void SSAORenderer::ReloadShaders()
	{
		ssaoShader_.Stop();
		ssaoShader_.Reload();
		ssaoShader_.Init();
	}
	void SSAORenderer::SSAOPass()
	{
		auto positionTexture = rendererContext_->defferedFrameBuffer_->GetTexture(0);

		ssaoShader_.Start();
		rendererContext_->graphicsApi_->ActiveTexture(0, positionTexture);
		rendererContext_->graphicsApi_->RenderQuad();
		ssaoShader_.Stop();
	}
	void SSAORenderer::BlurPass()
	{

	}
}
