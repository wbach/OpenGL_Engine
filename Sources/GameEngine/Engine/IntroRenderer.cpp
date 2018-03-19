#include "IntroRenderer.h"
#include "../Display/DisplayManager.hpp"
#include "../Shaders/Loading/LoadingShader.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
	IntroRenderer::IntroRenderer(GameEngine::IGraphicsApiPtr graphicsApi, std::shared_ptr<CDisplayManager>& displayManager)
		: graphicsApi_(graphicsApi)
		, displayManager_(displayManager)
		, resorceManager_(graphicsApi)
		, shader_(graphicsApi)
		, initialized_(false)
	{
	}
	IntroRenderer::~IntroRenderer()
	{
	}
	void IntroRenderer::Render()
	{
		if (displayManager_ == nullptr)
			return;

		if (!initialized_)
			Init();

		displayManager_->ProcessEvents();
		RenderThis();
		displayManager_->Update();
	}
	void IntroRenderer::Init()
	{
		shader_.Init();
		backgroundTexture_ = resorceManager_.GetTextureLaoder().LoadTextureImmediately("GUI/start1.png", false, ObjectTextureType::MATERIAL, TextureFlip::Type::VERTICAL);
		initialized_ = true;
	}

	void IntroRenderer::RenderThis()
	{
		shader_.Start();
		graphicsApi_->EnableDepthTest();
		graphicsApi_->PrepareFrame();
		renderQuad(mat4(1.f), backgroundTexture_->GetId());
		shader_.Stop();
	}

	void IntroRenderer::renderQuad(const glm::mat4 & transformMatrix, uint32 textureId) const
	{
		graphicsApi_->ActiveTexture(0, textureId);
		shader_.LoadTransformMatrix(transformMatrix);
		graphicsApi_->RenderQuad();
	}

} // GameEngine
