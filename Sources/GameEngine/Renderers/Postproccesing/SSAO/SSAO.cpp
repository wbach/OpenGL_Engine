#include "SSAO.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"

namespace GameEngine
{
	void SSAORenderer::Init()
	{
		blurRenderer_.SetRendererContext(rendererContext_);
		blurRenderer_.SetPostProcessFrameBuffer(postprocessFrameBuffer_);
		blurRenderer_.Init();

		ssaoShader_.reset(new SSAOShader(rendererContext_->graphicsApi_));
		ssaoShader_->Init();
		ssaoShader_->Start();
		GenKernel();
		ssaoShader_->Load(SSAOShader::UniformLocation::SampleRadius, 1.5f);
		ssaoShader_->Load(SSAOShader::UniformLocation::ProjectionMatrix, rendererContext_->projection_->GetProjectionMatrix());
		ssaoShader_->Stop();
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
		ssaoShader_->Stop();
		ssaoShader_->Reload();
		ssaoShader_->Init();
	}
	void SSAORenderer::SSAOPass()
	{
		auto positionTexture = rendererContext_->defferedFrameBuffer_->GetTexture(0);

		ssaoShader_->Start();
		rendererContext_->graphicsApi_->ActiveTexture(0, positionTexture);
		rendererContext_->graphicsApi_->RenderQuad();
		ssaoShader_->Stop();
	}
	void SSAORenderer::BlurPass()
	{
		blurRenderer_.Render(nullptr);
	}
	void SSAORenderer::GenKernel()
	{
		std::vector<vec3> kernel;
		kernel.resize(KERNEL_SIZE);

		for (uint32 i = 0; i < KERNEL_SIZE; i++) {
			float scale = (float)i / (float)(KERNEL_SIZE);
			vec3 v;
			v.x = 2.0f * (float)rand() / RAND_MAX - 1.0f;
			v.y = 2.0f * (float)rand() / RAND_MAX - 1.0f;
			v.z = 2.0f * (float)rand() / RAND_MAX - 1.0f;
			// Use an acceleration function so more points are
			// located closer to the origin
			v *= (0.1f + 0.9f * scale * scale);

			kernel[i] = v;
		}
		ssaoShader_->Load(SSAOShader::UniformLocation::Kernel, kernel);
	}
}
