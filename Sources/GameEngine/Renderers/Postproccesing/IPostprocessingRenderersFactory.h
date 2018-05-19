#pragma once
#include "PostprocessingRendererTypes.h"
#include <memory>

namespace GameEngine
{
	class PostprocessingRenderer;

	struct IPostprocessingRenderersFactory
	{
		virtual ~IPostprocessingRenderersFactory() {}
		virtual std::unique_ptr<PostprocessingRenderer> Create(PostprocessingRendererType type) = 0;
	};
} // GameEngine
