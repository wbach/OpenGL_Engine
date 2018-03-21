#pragma once
#include "../Renderer.h"
#include "Shaders/ParticlesShader.h"
#include "GameEngine/Api/IGraphicsApi.h"

class CProjection;

namespace GameEngine
{
	class ParticlesRenderer : public CRenderer
	{
	public:
		ParticlesRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection_matrix, CFrameBuffer* framebuffer);
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
		ParticlesShader shader;
		CProjection* projectionMatrix;
	};
} // GameEngine
