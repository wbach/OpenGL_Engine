#include "IntroRenderer.h"
#include "../Display/DisplayManager.hpp"
#include "../Resources/SimpleModels/Quad.h"
#include "../Shaders/Loading/LoadingShader.h"
#include "../Input/Input.h"
#include "GLM/GLMUtils.h"

namespace GameEngine
{
	IntroRenderer::IntroRenderer(std::shared_ptr<CDisplayManager>& displayManager)
		: displayManager_(displayManager)
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

		displayManager_->PeekApiMessage();
		RenderThis();
		displayManager_->Update();
	}
	void IntroRenderer::Init()
	{
		quad_.Init();
		shader_.Init();		
		backgroundTexture_ = resorceManager_.GetTextureLaoder().LoadTextureImmediately("GUI/start1.png", false, TextureType::MATERIAL, TextureFlip::Type::VERTICAL);
		initialized_ = true;
	}

	void IntroRenderer::RenderThis()
	{
		shader_.Start();
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.117f, 0.117f, .117f, 1.f);
		renderQuad(mat4(1.f), backgroundTexture_->GetId());
		shader_.Stop();
	}

	void IntroRenderer::renderQuad(const glm::mat4 & transformMatrix, uint32 textureId) const
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		shader_.LoadTransformMatrix(transformMatrix);
		Utils::SimpleRenderVao(quad_.vao, quad_.indicesSize, 2);
	}

} // GameEngine
