#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/Renderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include <memory>
#include <vector>

namespace GameEngine
{
	class PostprocessingRenderer : public CRenderer
	{
	public:
		PostprocessingRenderer(RendererContext& context);
		~PostprocessingRenderer();

		virtual void Init() override;
		virtual void PrepareFrame(GameEngine::Scene* scene) override;
		virtual void Render(GameEngine::Scene* scene) override;
		virtual void EndFrame(GameEngine::Scene* scene) override;
		virtual void Subscribe(CGameObject* gameObject) override;
		virtual void UnSubscribe(CGameObject* gameObject) override;
		virtual void UnSubscribeAll() override;
		virtual void ReloadShaders() override;

	private:
		void CreateRenderers();

	private:
		RendererContext& rendererContext_;
		std::vector<std::unique_ptr<CRenderer>> renderers_;
	};
}

