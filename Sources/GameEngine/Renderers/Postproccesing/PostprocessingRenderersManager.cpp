#include "PostprocessingRenderersManager.h"
#include "PostprocessingRenderer.h"
#include "PostprocessingRenderersFactory.h"
#include "GameEngine/Renderers/Projection.h"

namespace GameEngine
{
PostProcessingManager::PostProcessingManager(RendererContext& context)
	: context_(context)
	, postproccesFrameBuffer_(context.graphicsApi_)
{
	factory_ = std::make_unique<PostprocessingRenderersFactory>(context_, postproccesFrameBuffer_);
	
	AddEffect(PostprocessingRendererType::DEFFERED_LIGHT);
	AddEffect(PostprocessingRendererType::COLOR_FLIPER);
}
PostProcessingManager::~PostProcessingManager()
{
}
void PostProcessingManager::Init()
{
	postproccesFrameBuffer_.Init(context_.projection_->GetWindowSize());
	for (auto& renderer : postProcessingRenderers_)
	{
		renderer->Init();
	}
}
void PostProcessingManager::Render(Scene* scene)
{
	uint32 i = 0;
	for (auto& renderer : postProcessingRenderers_)
	{
		if (i == postProcessingRenderers_.size() - 1)
		{
			postproccesFrameBuffer_.UnBind();
		}
		else
		{
			postproccesFrameBuffer_.BindToDraw();
			
		}
		renderer->Render(scene);
		++i;
	}
}
void PostProcessingManager::ReloadShaders()
{
	for (auto& r : postProcessingRenderers_)
		r->ReloadShaders();
}
void PostProcessingManager::AddEffect(PostprocessingRendererType type)
{
	postProcessingRenderers_.push_back(std::move(factory_->Create(type)));
}
} // GameEngine
