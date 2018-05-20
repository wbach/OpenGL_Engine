#pragma once
#include "Shaders/ColorFliperShader.h"
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"

namespace GameEngine
{
	class ColorFliper : public PostprocessingRenderer
	{
	public:
		virtual void Init() override;
		virtual void Prepare(Scene*) override;
		virtual void Render(Scene*) override;
		virtual void ReloadShaders() override;

	private:
		std::unique_ptr<ColorFliperShader> shader_;
	};
} // GameEngine
