#pragma once
#include "Shaders/PlantsShader.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/Renderer.h"

class CProjection;

namespace GameEngine
{
	class PlantsRenderer : public CRenderer
	{
	public:
		PlantsRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection_matrix, CFrameBuffer* framebuffer);
		// Loading lights itp to shader
		virtual void Init() override;
		virtual void PrepareFrame(GameEngine::Scene* scene) override;
		virtual void Render(GameEngine::Scene* scene) override;
		virtual void EndFrame(GameEngine::Scene* scene) override;
		virtual void Subscribe(CGameObject* gameObject) override;
		virtual void UnSubscribe(CGameObject* gameObject) override;
		virtual void UnSubscribeAll() override;
		virtual void ReloadShaders() override;

	private:
		GameEngine::IGraphicsApiPtr graphicsApi_;
		PlantsShader shader;
		CProjection* projectionMatrix;
	};
} // GameEngine
