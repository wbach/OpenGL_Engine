#include "DefferedRenderer.h"

#include <Logger/Log.h>

#include "GameEngine/Renderers/BaseRenderer.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "Objects/Entity/EntityRenderer.h"
#include "Objects/Grass/GrassRenderer.h"
#include "Objects/Particles/ParticlesRenderer.h"
#include "Objects/Shadows/ShadowMapRenderer.hpp"
#include "Objects/SkyBox/SkyBoxRenderer.h"
#include "Objects/Terrain/Mesh/TerrainMeshRenderer.h"
#include "Objects/Terrain/TerrainRenderer.h"
#include "Objects/Tree/TreeRenderer.h"

namespace GameEngine
{
DefferedRenderer::DefferedRenderer(RendererContext& context)
    : BaseRenderer(context)
    , defferedFrameBuffer_(nullptr)
    , postprocessingRenderersManager_(context)
    , ssaoRenderer(context)
    , skyPassRenderer(context)
    , lightShaftRenderer_(context)
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
    if (not context_.camera_)
        return;

    createOrUpdateDefferedFrameBufferIfNeeded();

    if (isReady_)
    {
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
    LOG_DEBUG << "Init";
    context_.graphicsApi_.SetShaderQuaility(GraphicsApi::ShaderQuaility::FullDefferedRendering);

    createBaseRenderers();
    addRenderer<TerrainRenderer>();
    addRenderer<ParticlesRenderer>();
    addRenderer<GrassRenderer>();
    addRenderer<ShadowMapRenderer>();

    initRenderers();
    LOG_DEBUG << "DefferedRenderer renderers initialized.";
}

void DefferedRenderer::cleanUp()
{
    BaseRenderer::cleanUp();

    if (defferedFrameBuffer_)
    {
        context_.graphicsApi_.DeleteFrameBuffer(*defferedFrameBuffer_);
        defferedFrameBuffer_ = nullptr;
    }

    ssaoRenderer.CleanUp();
    skyPassRenderer.CleanUp();
    lightShaftRenderer_.CleanUp();
    postprocessingRenderersManager_.CleanUp();
}

void DefferedRenderer::reloadShaders()
{
    context_.graphicsApi_.SetShaderQuaility(GraphicsApi::ShaderQuaility::FullDefferedRendering);
    for (auto& renderer : renderers)
        renderer.ptr->reloadShaders();

    postprocessingRenderersManager_.ReloadShaders();
    skyPassRenderer.ReloadShaders();
    ssaoRenderer.ReloadShaders();
    lightShaftRenderer_.ReloadShaders();
}
void DefferedRenderer::setViewPort()
{
    const auto& renderingSize = context_.camera_->GetProjection().GetRenderingSize();
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
    {
        if (auto normalTextureId = defferedFrameBuffer_->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color2))
        {
            ssaoRenderer.Render(*depthTextureId, *normalTextureId);
        }
        skyPassRenderer.Render(*depthTextureId);
        lightShaftRenderer_.Render(*depthTextureId);
    }

    postprocessingRenderersManager_.Render(*defferedFrameBuffer_, renderTarget, *context_.scene_);
}

void DefferedRenderer::createFrameBuffer()
{
    using namespace GraphicsApi::FrameBuffer;
    defferedFrameBufferSize_ = context_.camera_->GetProjection().GetRenderingSize();
    Attachment worldPositionAttachment(*defferedFrameBufferSize_, Type::Color0, Format::Rgba32f);
    Attachment diffuseAttachment(*defferedFrameBufferSize_, Type::Color1, Format::Rgba32f);
    Attachment normalAttachment(*defferedFrameBufferSize_, Type::Color2, Format::Rgba32f, vec4(0.f, 0.f, 0.f, 1.f));
    Attachment specularAttachment(*defferedFrameBufferSize_, Type::Color3, Format::Rgba8);
    Attachment depthAttachment(*defferedFrameBufferSize_, Type::Depth, Format::Depth);

    defferedFrameBuffer_ = &context_.graphicsApi_.CreateFrameBuffer(
        {worldPositionAttachment, diffuseAttachment, normalAttachment, specularAttachment, depthAttachment});
    isReady_ = defferedFrameBuffer_->Init();

    LOG_DEBUG << "DefferedRenderer Framebuffer created. Status : " << isReady_ << ", size : " << defferedFrameBufferSize_;
}

void DefferedRenderer::createOrUpdateDefferedFrameBufferIfNeeded()
{
    if (not defferedFrameBuffer_)
    {
        context_.graphicsApi_.SetShaderQuaility(GraphicsApi::ShaderQuaility::FullDefferedRendering);
        createFrameBuffer();
        ssaoRenderer.Init();
        skyPassRenderer.Init();
        lightShaftRenderer_.Init();
        postprocessingRenderersManager_.Init();
        return;
    }

    const auto& renderingSize = context_.camera_->GetProjection().GetRenderingSize();
    if (defferedFrameBufferSize_.value() == renderingSize)
    {
        return;
    }

    LOG_DEBUG << "Update frame buffer size from: " << defferedFrameBufferSize_ << " to " << renderingSize;

    defferedFrameBufferSize_ = renderingSize;
    context_.graphicsApi_.DeleteFrameBuffer(*defferedFrameBuffer_);
    createFrameBuffer();
    context_.graphicsApi_.SetShaderQuaility(GraphicsApi::ShaderQuaility::FullDefferedRendering);

    skyPassRenderer.CleanUp();
    skyPassRenderer.Init();

    lightShaftRenderer_.CleanUp();
    lightShaftRenderer_.Init();

    ssaoRenderer.CleanUp();
    ssaoRenderer.Init();

    postprocessingRenderersManager_.OnSizeChanged();
}

}  // namespace GameEngine
