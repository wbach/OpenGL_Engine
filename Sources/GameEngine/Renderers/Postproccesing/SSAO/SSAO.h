#pragma once
#include "GameEngine/Renderers/Renderer.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "Shaders/SSAOShader.h"

class CProjection;

namespace GameEngine
{
	class SSAORenderer : public CRenderer
	{
	public:
		SSAORenderer(IGraphicsApiPtr graphicsApi, CProjection* projection, CFrameBuffer* frambuffer);
		virtual void Init() override;
		virtual void PrepareFrame(GameEngine::Scene* scene) override;
		virtual void Render(GameEngine::Scene* scene) override;
		virtual void EndFrame(GameEngine::Scene* scene) override;
		virtual void ReloadShaders() override;

	private:
		GameEngine::IGraphicsApiPtr graphicsApi_;
		CProjection* projection_;
		SSAOShader shader_;
	};
}

