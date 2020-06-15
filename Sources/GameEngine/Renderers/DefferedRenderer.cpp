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
    , resizeRenderingMode_(false)
    , isReady_(false)
{
}

DefferedRenderer::~DefferedRenderer()
{
    DEBUG_LOG("");

    if (defferedFrameBuffer_)
        context_.graphicsApi_.DeleteFrameBuffer(*defferedFrameBuffer_);
}

void DefferedRenderer::render()
{
    if (isReady_)
    {
        bindDefferedFbo();
        BaseRenderer::render();
        unbindDefferedFbo();
    }
}

void DefferedRenderer::init()
{
    context_.graphicsApi_.SetShaderQuaility(GraphicsApi::ShaderQuaility::FullDefferedRendering);

    resizeRenderingMode_ = (context_.projection_.GetRenderingSize().x != EngineConf.window.size.x or
                            context_.projection_.GetRenderingSize().y != EngineConf.window.size.y);

    createFrameBuffer();
    createRenderers();

    DEBUG_LOG("Rendering size : " + std::to_string(context_.projection_.GetRenderingSize()) +
              ", resizeRenderingMode : " + Utils::BoolToString(resizeRenderingMode_));

    initRenderers();
    postprocessingRenderersManager_.Init();

    DEBUG_LOG("DefferedRenderer initialized.");
}

void DefferedRenderer::reloadShaders()
{
    BaseRenderer::reloadShaders();
    postprocessingRenderersManager_.ReloadShaders();
}

void DefferedRenderer::createRenderers()
{
    BaseRenderer::createRenderers();

    if (EngineConf.renderer.shadows.isEnabled)
        addRenderer<ShadowMapRenderer>();
}
void DefferedRenderer::bindDefferedFbo()
{
    defferedFrameBuffer_->Clear();
    defferedFrameBuffer_->Bind(GraphicsApi::FrameBuffer::BindType::Write);

    if (resizeRenderingMode_)
    {
        const auto& renderingSize = context_.projection_.GetRenderingSize();
        context_.graphicsApi_.SetViewPort(0, 0, renderingSize.x, renderingSize.y);
    }
}
void DefferedRenderer::unbindDefferedFbo()
{
    if (resizeRenderingMode_)
    {
        const auto& windowSize = EngineConf.window.size;
        context_.graphicsApi_.SetViewPort(0, 0, windowSize.x, windowSize.y);
    }

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
