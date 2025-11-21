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
DefferedRenderer::DefferedRenderer(RendererContext& context, GraphicsApi::IFrameBuffer& renderTarget)
    : BaseRenderer(context, renderTarget)
    , defferedFrameBuffer_(nullptr)
    , postprocessingRenderersManager_(context, renderTarget)
    , skyPassRenderer(context)
    , isReady_(false)
{
}

DefferedRenderer::~DefferedRenderer()
{
    LOG_DEBUG << "destructor";

    if (defferedFrameBuffer_)
        context_.graphicsApi_.DeleteFrameBuffer(*defferedFrameBuffer_);
}

void DefferedRenderer::render()
{
    if (isReady_)
    {
        updateDefferedFrameBufferIfNeeded();

        bindDefferedFbo();
        context_.graphicsApi_.EnableDepthTest();
        context_.graphicsApi_.EnableDepthMask();
        BaseRenderer::renderImpl();
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

    initRenderers();
    postprocessingRenderersManager_.Init();

    skyPassRenderer.Init();

    LOG_DEBUG << "DefferedRenderer initialized.";
}

void DefferedRenderer::reloadShaders()
{
    BaseRenderer::reloadShaders();
    postprocessingRenderersManager_.ReloadShaders();
    skyPassRenderer.ReloadShaders();
}
void DefferedRenderer::setViewPort()
{
    const auto& renderingSize = context_.projection_.GetRenderingSize();
    context_.graphicsApi_.SetViewPort(0, 0, renderingSize.x, renderingSize.y);
}
void DefferedRenderer::bindDefferedFbo()
{
    defferedFrameBuffer_->Clear();
    defferedFrameBuffer_->Bind(GraphicsApi::FrameBuffer::BindType::Write);
}
void DefferedRenderer::unbindDefferedFbo()
{
    if (auto depthTextureId = defferedFrameBuffer_->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Depth))
        skyPassRenderer.Render(*depthTextureId);
    postprocessingRenderersManager_.Render(*defferedFrameBuffer_, *context_.scene_);
}

void DefferedRenderer::createFrameBuffer()
{
    using namespace GraphicsApi::FrameBuffer;
    defferedFrameBufferSize_ = context_.projection_.GetRenderingSize();
    Attachment worldPositionAttachment(*defferedFrameBufferSize_, Type::Color0, Format::Rgba32f);
    Attachment diffuseAttachment(*defferedFrameBufferSize_, Type::Color1, Format::Rgba32f);
    Attachment normalAttachment(*defferedFrameBufferSize_, Type::Color2, Format::Rgba32f, vec4(0.f, 0.f, 0.f, 1.f));
    Attachment specularAttachment(*defferedFrameBufferSize_, Type::Color3, Format::Rgba8);
    Attachment depthAttachment(*defferedFrameBufferSize_, Type::Depth, Format::Depth);

    defferedFrameBuffer_ = &context_.graphicsApi_.CreateFrameBuffer(
        {worldPositionAttachment, diffuseAttachment, normalAttachment, specularAttachment, depthAttachment});
    isReady_ = defferedFrameBuffer_->Init();
}

void DefferedRenderer::updateDefferedFrameBufferIfNeeded()
{
    if (not defferedFrameBufferSize_ or not defferedFrameBuffer_)
        return;

    if (defferedFrameBufferSize_.value() == context_.projection_.GetRenderingSize())
    {
        return;
    }

    LOG_DEBUG << "Update frame buffer size from: " << defferedFrameBufferSize_ << " to "
              << context_.projection_.GetRenderingSize();

    defferedFrameBufferSize_ = context_.projection_.GetRenderingSize();
    context_.graphicsApi_.DeleteFrameBuffer(*defferedFrameBuffer_);
    createFrameBuffer();

    skyPassRenderer.CleanUp();
    skyPassRenderer.Init();

    postprocessingRenderersManager_.OnSizeChanged();
}

}  // namespace GameEngine
