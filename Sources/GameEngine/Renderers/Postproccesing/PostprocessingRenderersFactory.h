#pragma once
#include "IPostprocessingRenderersFactory.h"

namespace GameEngine
{
	struct RendererContext;
	class PostprocessFrameBuffer;

	class PostprocessingRenderersFactory : public IPostprocessingRenderersFactory
	{
	public:
		PostprocessingRenderersFactory(RendererContext& context, PostprocessFrameBuffer** postprocessFrameBuffer);
		virtual std::unique_ptr<PostprocessingRenderer> Create(PostprocessingRendererType type) override;

	private:
		template<class T>
		std::unique_ptr<T> CreateAndBasicInitialize();

	private:
		RendererContext& context_;
		PostprocessFrameBuffer** postprocessFrameBuffer_;
	};
} // GameEngine
