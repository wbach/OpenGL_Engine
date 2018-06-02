#include "ColorFliper.h"

namespace GameEngine
{
	void ColorFliper::Init()
	{
		shader_.reset(new ColorFliperShader(rendererContext_->graphicsApi_));
		shader_->Init();
	}
	void ColorFliper::Prepare(Scene*)
	{
	}
	void ColorFliper::Render(Scene*)
	{
		shader_->Start();
		(*postprocessFrameBuffer_)->BindTextures();
		rendererContext_->graphicsApi_->RenderQuad();
		shader_->Stop();
	}
	void ColorFliper::ReloadShaders()
	{
		shader_->Stop();
		Init();
	}
} // GameEngine
