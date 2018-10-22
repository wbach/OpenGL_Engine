#pragma once
#include "../Resources/ResourceManager.h"
#include "../Shaders/Loading/LoadingShader.h"
#include "Types.h"
#include <memory>

namespace GameEngine
{
	class CDisplayManager;

	class IntroRenderer
	{
	public:
		IntroRenderer(GameEngine::IGraphicsApiPtr graphicsApi, std::shared_ptr<CDisplayManager>& displayManager);
		~IntroRenderer();
		void Render();

	private:
		void Init();
		void RenderThis();
		void renderQuad(const glm::mat4 & transformMatrix, uint32 textureId) const;

	private:
		GameEngine::IGraphicsApiPtr graphicsApi_;
		std::shared_ptr<CDisplayManager>& displayManager_;
		ResourceManager resorceManager_;

		CLoadingShader shader_;
		CTexture* backgroundTexture_;

		bool initialized_;
	};
} // GameEngine