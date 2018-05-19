#pragma once
#include "Shaders/WaterShader.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"

class CProjection;

namespace GameEngine
{
	class WaterRenderer : public IRenderer
	{
	public:
		WaterRenderer(RendererContext& context);
		virtual void Init() override;
		virtual void Subscribe(CGameObject* gameObject) override;
		virtual void UnSubscribe(CGameObject* gameObject) override;
		virtual void UnSubscribeAll() override;
		virtual void ReloadShaders() override;
		void Render(Scene* scene);

	private:
		RendererContext& context_;
		WaterShader shader;
	};
} // GameEngine
