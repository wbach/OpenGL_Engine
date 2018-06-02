#pragma once
#include "Shaders/BlurShader.h"
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"

namespace GameEngine
{
	class Blur : public PostprocessingRenderer
	{
	public:
		virtual void Init() override;
		virtual void Prepare(Scene*) override;
		virtual void Render(Scene*) override;
		virtual void ReloadShaders() override;

	private:
		std::unique_ptr<BlurShader> shader_;
	};
} // GameEngine
