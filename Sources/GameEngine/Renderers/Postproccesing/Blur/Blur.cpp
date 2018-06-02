#include "Blur.h"

namespace GameEngine
{
	void Blur::Init()
	{
		shader_.reset(new BlurShader(rendererContext_->graphicsApi_));
		shader_->Init();
	}
	void Blur::Prepare(Scene *)
	{
	}
	void Blur::Render(Scene *)
	{
		shader_->Start();
		(*postprocessFrameBuffer_)->BindTextures();
		rendererContext_->graphicsApi_->RenderQuad();
		shader_->Stop();
	}
	void Blur::ReloadShaders()
	{
		shader_->Stop();
		shader_->Reload();
		shader_->Init();
	}
} // GameEngine
