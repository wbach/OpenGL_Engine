#include "PostprocessingRenderersFactory.h"
#include "DefferedLighting/DefferedLighting.h"

namespace GameEngine
{
	PostprocessingRenderersFactory::PostprocessingRenderersFactory(RendererContext& context)
		: context_(context)
	{
	}
	std::unique_ptr<PostprocessingRenderer> PostprocessingRenderersFactory::Create(PostprocessingRendererType type)
	{
		switch (type)
		{
		case PostprocessingRendererType::DEFFERED_LIGHT: return CreateAndBasicInitialize<DefferedLighting>();
		}
		return nullptr;
	}
	template<class T>
	std::unique_ptr<T> PostprocessingRenderersFactory::CreateAndBasicInitialize()
	{
		auto comp = std::make_unique<T>();
		comp->SetRendererContext(&context_);
		return comp;
	}
} // GameEngine
