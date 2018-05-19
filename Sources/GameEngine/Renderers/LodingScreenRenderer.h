#pragma once
#include "../Shaders/Loading/LoadingShader.h"
#include "IRenderer.h"

class CTexture;

namespace GameEngine
{
	class LoadingScreenRenderer : public IRenderer
	{
	public:
		LoadingScreenRenderer(IGraphicsApiPtr, CTexture* bgTexture, CTexture* circleTexture);
		virtual void Init() override;
		void Render(Scene*);
		virtual void ReloadShaders() override {}

	private:
		void prepareRender();
		void renderQuad(const glm::mat4& transformMatrix, uint32 textureId) const;

	private:
		IGraphicsApiPtr graphicsApi_;
		CLoadingShader loadingShader;
		CTexture* circleTexture;
		CTexture* backgroundTexture;
		mat4 transformationMatrix;
	};
} // GameEngine
