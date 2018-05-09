#pragma once
#include "GameEngine/Api/IGraphicsApi.h"
#include "GameEngine/Renderers/Renderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include <memory>
#include <vector>

namespace GameEngine
{
	class PostprocessingRenderer
	{
	public:
		virtual ~PostprocessingRenderer();

		virtual void Init() = 0;
		virtual void Prepare() = 0;
		virtual void Render() = 0;
		virtual void ReloadShaders() = 0;
		void SetRendererContext(RendererContext* rendererContext);

	protected:
		RendererContext* rendererContext_;
	};

	typedef std::unique_ptr<PostprocessingRenderer> PostprocessingRendererPtr;
} // GameEngine
