#pragma once
#include "IPostprocessingRenderersFactory.h"

namespace GameEngine
{
	struct RendererContext;
	class PostprocessingRenderersFactory : public IPostprocessingRenderersFactory
	{
	public:
		PostprocessingRenderersFactory(RendererContext& context);
		virtual std::unique_ptr<PostprocessingRenderer> Create(PostprocessingRendererType type) override;

	private:
		template<class T>
		std::unique_ptr<T> CreateAndBasicInitialize();

	private:
		RendererContext& context_;
	};
} // GameEngine
