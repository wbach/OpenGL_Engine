#pragma once
#include "GameEngine/Renderers/Postproccesing/PostprocessingRenderer.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "Shaders/SSAOShader.h"

class CProjection;

namespace GameEngine
{
	class SSAORenderer : public PostprocessingRenderer
	{
	public:
		virtual void Init() override;
		virtual void Prepare(Scene* scene) override;
		virtual void Render(Scene* scene) override;
		virtual void ReloadShaders() override;


	private:
		void SSAOPass();
		void BlurPass();

	private:
		SSAOShader ssaoShader_;
	};
}

