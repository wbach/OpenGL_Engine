#include "PostprocessingRenderersFactory.h"
#include "DefferedLighting/DefferedLighting.h"
#include "ColorFliper/ColorFliper.h"

namespace GameEngine
{
	PostprocessingRenderersFactory::PostprocessingRenderersFactory(RendererContext& context, PostprocessFrameBuffer& postprocessFrameBuffer)
		: context_(context)
		, postprocessFrameBuffer_(postprocessFrameBuffer)
	{
	}
	std::unique_ptr<PostprocessingRenderer> PostprocessingRenderersFactory::Create(PostprocessingRendererType type)
	{
		switch (type)
		{
		case PostprocessingRendererType::DEFFERED_LIGHT: return CreateAndBasicInitialize<DefferedLighting>();
		case PostprocessingRendererType::COLOR_FLIPER: return CreateAndBasicInitialize<ColorFliper>();
		}
		return nullptr;
	}
	template<class T>
	std::unique_ptr<T> PostprocessingRenderersFactory::CreateAndBasicInitialize()
	{
		auto comp = std::make_unique<T>();
		comp->SetRendererContext(&context_);
		comp->SetPostProcessFrameBuffer(&postprocessFrameBuffer_);
		return comp;
	}
} // GameEngine
