#include "PostprocessingRenderersManager.h"
#include "PostprocessingRenderer.h"
#include "PostprocessingRenderersFactory.h"
#include "GameEngine/Renderers/Projection.h"

namespace GameEngine
{
PostProcessingManager::PostProcessingManager(RendererContext& context)
	: context_(context)
	, first(false)
	, postproccesFrameBuffer1_(context.graphicsApi_)
	, postproccesFrameBuffer2_(context.graphicsApi_)
{
	activePostProcessFrameBuffer_ = &postproccesFrameBuffer1_;
	passivePostProcessFrameBuffer_ = &postproccesFrameBuffer2_;

	factory_ = std::make_unique<PostprocessingRenderersFactory>(context_, &passivePostProcessFrameBuffer_);
	
	AddEffect(PostprocessingRendererType::DEFFERED_LIGHT);
	AddEffect(PostprocessingRendererType::COLOR_FLIPER);
	AddEffect(PostprocessingRendererType::BLUR);
}
PostProcessingManager::~PostProcessingManager()
{
}
void PostProcessingManager::Init()
{
	postproccesFrameBuffer1_.Init(context_.projection_->GetWindowSize());
	postproccesFrameBuffer2_.Init(context_.projection_->GetWindowSize());

	for (auto& renderer : postProcessingRenderers_)
	{
		renderer->Init();
	}
}
void PostProcessingManager::Render(Scene* scene)
{
	uint32 i = 0;
	first = true;
	for (auto& renderer : postProcessingRenderers_)
	{
		if (i == postProcessingRenderers_.size() - 1)
		{
			activePostProcessFrameBuffer_->UnBind();
		}
		else
		{
			activePostProcessFrameBuffer_->BindToDraw();
		}
		renderer->Render(scene);

		if (first)
		{
			activePostProcessFrameBuffer_ = &postproccesFrameBuffer2_;
			passivePostProcessFrameBuffer_ = &postproccesFrameBuffer1_;
		}
		else
		{
			activePostProcessFrameBuffer_ = &postproccesFrameBuffer1_;
			passivePostProcessFrameBuffer_ = &postproccesFrameBuffer2_;
		}

		first = !first;
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
