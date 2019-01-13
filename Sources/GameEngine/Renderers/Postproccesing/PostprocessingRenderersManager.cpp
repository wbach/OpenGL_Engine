#include "PostprocessingRenderersManager.h"
#include "GameEngine/Renderers/Projection.h"
#include "PostprocessingRenderer.h"
#include "PostprocessingRenderersFactory.h"

namespace GameEngine
{
PostProcessingManager::PostProcessingManager(RendererContext& context)
    : context_(context)
    , first_(false)
    , postproccesFrameBuffer1_(context.graphicsApi_)
    , postproccesFrameBuffer2_(context.graphicsApi_)
    , ambientOclusionFrameBuffer_(context.graphicsApi_)
{
    ResetBufferSet();
    factory_      = std::make_unique<PostprocessingRenderersFactory>(context_, &passivePostProcessFrameBuffer_);
    ssaoRenderer_ = factory_->Create(PostprocessingRendererType::SSAO);
    AddEffects();
}
PostProcessingManager::~PostProcessingManager()
{
}
void PostProcessingManager::Init()
{
    postproccesFrameBuffer1_.Init(context_.projection_.GetWindowSize());
    postproccesFrameBuffer2_.Init(context_.projection_.GetWindowSize());
    ambientOclusionFrameBuffer_.Init(context_.projection_.GetWindowSize());

    for (auto& renderer : postProcessingRenderers_)
    {
        renderer->Init();
    }
    ssaoRenderer_->Init();
}
void PostProcessingManager::Render(Scene* scene)
{
    uint32 i = 0;
    first_   = true;
    ResetBufferSet();

    //	ambientOclusionFrameBuffer_.BindToDraw();
    //	ssaoRenderer_->Render(scene);
    //	ambientOclusionFrameBuffer_.UnBind();

    for (auto& renderer : postProcessingRenderers_)
    {
        BindBuffer(i++);
        renderer->Render(scene);
        SwapBuffers();
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
void PostProcessingManager::ResetBufferSet()
{
    activePostProcessFrameBuffer_  = &postproccesFrameBuffer1_;
    passivePostProcessFrameBuffer_ = &postproccesFrameBuffer2_;
}
void PostProcessingManager::SwapBuffers()
{
    if (first_)
    {
        activePostProcessFrameBuffer_  = &postproccesFrameBuffer2_;
        passivePostProcessFrameBuffer_ = &postproccesFrameBuffer1_;
    }
    else
    {
        activePostProcessFrameBuffer_  = &postproccesFrameBuffer1_;
        passivePostProcessFrameBuffer_ = &postproccesFrameBuffer2_;
    }

    first_ = !first_;
}
void PostProcessingManager::BindBuffer(uint32 i)
{
    if (IsLastRenderer(i))
    {
        activePostProcessFrameBuffer_->UnBindDraw();
    }
    else
    {
        activePostProcessFrameBuffer_->BindToDraw();
    }
}
bool PostProcessingManager::IsLastRenderer(uint32 i)
{
    return i == postProcessingRenderers_.size() - 1;
}
void PostProcessingManager::AddEffects()
{
    AddEffect(PostprocessingRendererType::DEFFERED_LIGHT);
    // AddEffect(PostprocessingRendererType::COLOR_FLIPER);
    // AddEffect(PostprocessingRendererType::BLUR);
}
}  // namespace GameEngine
