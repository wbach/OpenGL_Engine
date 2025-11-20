#include "PostprocessingRenderersManager.h"

#include <GraphicsApi/BufferParamters.h>
#include <Logger/Log.h>

#include "FrameBuffersManager.h"
#include "GameEngine/Renderers/Postproccesing/IPostprocessingRenderersFactory.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "PostprocessingRenderer.h"
#include "PostprocessingRenderersFactory.h"

namespace GameEngine
{
PostProcessingManager::PostProcessingManager(RendererContext& context)
    : context_(context)
{
    factory_ = std::make_unique<PostprocessingRenderersFactory>(context_);
    AddEffects();
}
PostProcessingManager::~PostProcessingManager()
{
    fboManager_.reset();
}
void PostProcessingManager::Init()
{
    fboManager_ = std::make_unique<FrameBuffersManager>(context_.graphicsApi_, context_.projection_.GetRenderingSize());

    if (fboManager_->GetStatus())
    {
        for (auto& renderer : postProcessingRenderers_)
        {
            renderer->Init();
        }
    }
    else
    {
        LOG_DEBUG << "Buffer creation error.";
    }
}

void PostProcessingManager::OnSizeChanged()
{
    fboManager_.reset();

    fboManager_ = std::make_unique<FrameBuffersManager>(context_.graphicsApi_, context_.projection_.GetRenderingSize());
}

void PostProcessingManager::Render(GraphicsApi::IFrameBuffer& startedFrameBuffer, const Scene& scene)
{
    if (not fboManager_->GetStatus() or postProcessingRenderers_.empty())
    {
        LOG_DEBUG << "No activePostProcessing effects.";
        return;
    }
    fboManager_->StartFrame();

    if (IsLastRenderer(0))
    {
        bindDefaultFrameBuffer();
    }
    else
    {
        fboManager_->BindForWriting();
    }

    startedFrameBuffer.Bind(GraphicsApi::FrameBuffer::BindType::Textures);
    postProcessingRenderers_[0]->Render(scene);
    fboManager_->Swap();

    for (size_t index = 1; index < postProcessingRenderers_.size(); ++index)
    {
        if (IsLastRenderer(index))
        {
            bindDefaultFrameBuffer();
        }
        else
        {
            fboManager_->BindForWriting();
        }

        fboManager_->BindForReading();
        postProcessingRenderers_[index]->Render(scene);
        fboManager_->Swap();
    }
}
void PostProcessingManager::ReloadShaders()
{
    for (auto& r : postProcessingRenderers_)
        r->ReloadShaders();
}
void PostProcessingManager::AddEffect(PostprocessingRendererType type)
{
    postProcessingRenderers_.push_back(factory_->Create(type));
}
bool PostProcessingManager::IsLastRenderer(size_t i)
{
    return i == postProcessingRenderers_.size() - 1;
}
void PostProcessingManager::AddEffects()
{
    AddEffect(PostprocessingRendererType::DEFFERED_LIGHT);
    AddEffect(PostprocessingRendererType::TONE_MAPPING);
    // AddEffect(PostprocessingRendererType::COLOR_FLIPER);c
    // AddEffect(PostprocessingRendererType::BLUR);
    // AddEffect(PostprocessingRendererType::OUTLINE);
    AddEffect(PostprocessingRendererType::FXAA);
}
void PostProcessingManager::bindDefaultFrameBuffer()
{
    const auto windowSize = context_.graphicsApi_.GetWindowApi().GetWindowSize();

    if (context_.projection_.GetRenderingSize() != windowSize)
    {
        context_.graphicsApi_.SetViewPort(0, 0, windowSize.x, windowSize.y);
    }

    context_.graphicsApi_.GetDefaultFrameBuffer().Bind();
}
}  // namespace GameEngine
