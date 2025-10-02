#include "DefferedRenderer.h"

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "Logger/Log.h"
#include "Objects/Entity/EntityRenderer.h"
#include "Objects/Grass/GrassRenderer.h"
#include "Objects/Particles/ParticlesRenderer.h"
#include "Objects/Shadows/ShadowMapRenderer.hpp"
#include "Objects/SkyBox/SkyBoxRenderer.h"
#include "Objects/Skydome/SkydomeRenderer.h"
#include "Objects/Terrain/Mesh/TerrainMeshRenderer.h"
#include "Objects/Terrain/TerrainRenderer.h"
#include "Objects/Tree/TreeRenderer.h"
#include "Objects/Water/WaterRenderer.h"

namespace GameEngine
{
DefferedRenderer::DefferedRenderer(RendererContext& context)
    : BaseRenderer(context)
    , defferedFrameBuffer_(nullptr)
    , postprocessingRenderersManager_(context)
    , isReady_(false)
{
    windowSizeSubscribtionChange_ = EngineConf.window.size.subscribeForChange(
        [this]()
        {
            LOG_DEBUG << "Resize mode enabled. Window size :  " << EngineConf.window.size.toString()
                      << ", rendering size : " << context_.projection_.GetRenderingSize();
        });
}

DefferedRenderer::~DefferedRenderer()
{
    LOG_DEBUG << "destructor";

    EngineConf.window.size.unsubscribe(windowSizeSubscribtionChange_);
    if (defferedFrameBuffer_)
        context_.graphicsApi_.DeleteFrameBuffer(*defferedFrameBuffer_);
}

void DefferedRenderer::render()
{
    if (isReady_)
    {
        bindDefferedFbo();
        context_.graphicsApi_.EnableDepthTest();
        context_.graphicsApi_.EnableDepthMask();
        BaseRenderer::render();
        context_.graphicsApi_.DisableDepthMask();
        context_.graphicsApi_.DisableDepthTest();
        unbindDefferedFbo();
    }
}

void DefferedRenderer::init()
{
    context_.graphicsApi_.SetShaderQuaility(GraphicsApi::ShaderQuaility::FullDefferedRendering);

    createFrameBuffer();
    createRenderers();

    const auto windowSize = context_.graphicsApi_.GetWindowApi().GetWindowSize();
    if (context_.projection_.GetRenderingSize() != windowSize)
    {
        LOG_DEBUG << "Resize mode enabled. Window size :  " << windowSize
                  << ", rendering size : " << context_.projection_.GetRenderingSize();
    }

    initRenderers();
    postprocessingRenderersManager_.Init();

    LOG_DEBUG << "DefferedRenderer initialized.";
}

void DefferedRenderer::reloadShaders()
{
    BaseRenderer::reloadShaders();
    postprocessingRenderersManager_.ReloadShaders();
}
void DefferedRenderer::setViewPort()
{
    const auto windowSize = context_.graphicsApi_.GetWindowApi().GetWindowSize();
    if (context_.projection_.GetRenderingSize() != windowSize)
    {
        const auto& renderingSize = context_.projection_.GetRenderingSize();
        context_.graphicsApi_.SetViewPort(0, 0, renderingSize.x, renderingSize.y);
    }
    else
    {
        context_.graphicsApi_.SetViewPort(0, 0, windowSize.x, windowSize.y);
    }
}
void DefferedRenderer::bindDefferedFbo()
{
    defferedFrameBuffer_->Clear();
    defferedFrameBuffer_->Bind(GraphicsApi::FrameBuffer::BindType::Write);
}
void DefferedRenderer::unbindDefferedFbo()
{
    postprocessingRenderersManager_.Render(*defferedFrameBuffer_, *context_.scene_);
}

void DefferedRenderer::createFrameBuffer()
{
    using namespace GraphicsApi::FrameBuffer;
    const auto& size = context_.projection_.GetRenderingSize();
    Attachment worldPositionAttachment(size, Type::Color0, Format::Rgba32f);
    Attachment diffuseAttachment(size, Type::Color1, Format::Rgba8);
    Attachment normalAttachment(size, Type::Color2, Format::Rgba32f, vec4(0.f, 0.f, 0.f, 1.f));
    Attachment specularAttachment(size, Type::Color3, Format::Rgba8);
    Attachment depthAttachment(size, Type::Depth, Format::Depth);

    defferedFrameBuffer_ = &context_.graphicsApi_.CreateFrameBuffer(
        {worldPositionAttachment, diffuseAttachment, normalAttachment, specularAttachment, depthAttachment});
    isReady_ = defferedFrameBuffer_->Init();
}
}  // namespace GameEngine
