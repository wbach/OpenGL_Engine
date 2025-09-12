#include "ShadowMapRenderer.hpp"

#include <GLM/GLMUtils.h>
#include <GraphicsApi/ShaderProgramType.h>
#include <Logger/Log.h>

#include <math.hpp>

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
ShadowMapRenderer::ShadowMapRenderer(RendererContext& context)
    : context_(context)
    , entityRenderer_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Shadows)
    , instancedShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::InstancesShadows)
    , shadowBox_(context.projection_)
    , projectionViewMatrix_(1.f)
    , biasMatrix_(Utils::CreateBiasNdcToTextureCoordinates())
    , isActive_{EngineConf.renderer.shadows.isEnabled}
{
    for (uint32 cascadeIndex = 0; cascadeIndex < Params::MAX_SHADOW_MAP_CASADES; ++cascadeIndex)
    {
        shadowFrameBuffer_[cascadeIndex] = nullptr;
    }

    shadowEnabledSubscriptionId_ = EngineConf.renderer.shadows.isEnabled.subscribeForChange(
        [this]()
        {
            context_.gpuLoader_.AddFunctionToCall(
                [this]()
                {
                    if (EngineConf.renderer.shadows.isEnabled)
                    {
                        if (not isInit())
                            init();

                        isActive_ = true;
                    }
                    else
                    {
                        isActive_ = false;
                        cleanUp();
                    }
                });
        });
}

ShadowMapRenderer::~ShadowMapRenderer()
{
    EngineConf.renderer.shadows.isEnabled.unsubscribe(shadowEnabledSubscriptionId_);
    cleanUp();
}

void ShadowMapRenderer::init()
{
    shader_.Init();
    instancedShader_.Init();

    if (not isActive_ or not shader_.IsReady())
        return;

    entityRenderer_.init();

    GraphicsApi::FrameBuffer::Attachment depthAttachment(vec2ui(*EngineConf.renderer.shadows.mapSize),
                                                         GraphicsApi::FrameBuffer::Type::Depth,
                                                         GraphicsApi::FrameBuffer::Format::Depth);

    depthAttachment.wrapMode    = GraphicsApi::FrameBuffer::WrapMode::ClampToEdge;
    depthAttachment.filter      = GraphicsApi::FrameBuffer::Filter::Linear;
    depthAttachment.compareMode = GraphicsApi::FrameBuffer::CompareMode::RefToTexture;

    perFrameBuffer_ = context_.graphicsApi_.CreateShaderBuffer(PER_FRAME_BIND_LOCATION, sizeof(PerFrameBuffer));

    if (not perFrameBuffer_)
    {
        LOG_ERROR << "Shadow perframebuffer creation error.";
        shader_.Clear();
        return;
    }

    for (uint32 cascadeIndex = 0; cascadeIndex < Params::MAX_SHADOW_MAP_CASADES; ++cascadeIndex)
    {
        auto& shadowFrameBuffer = shadowFrameBuffer_[cascadeIndex];
        shadowFrameBuffer       = &context_.graphicsApi_.CreateFrameBuffer({depthAttachment});

        if (not context_.shadowsBufferId_)
            context_.shadowsBufferId_ =
                context_.graphicsApi_.CreateShaderBuffer(SHADOW_BUFFER_BIND_LOCATION, sizeof(ShadowsBuffer));
        if (context_.shadowsBufferId_)
            context_.graphicsApi_.BindShaderBuffer(*context_.shadowsBufferId_);

        auto status = shadowFrameBuffer->Init();

        if (not status)
        {
            context_.graphicsApi_.DeleteFrameBuffer(*shadowFrameBuffer);
            shadowFrameBuffer = nullptr;
            LOG_ERROR << "Shadow framebuffer creation error.";
            shader_.Clear();
            return;
        }

        context_.cascadedShadowMapsIds_[cascadeIndex] =
            shadowFrameBuffer->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Depth);
    }

    // relevant to shadowbox
    const auto& distances = shadowBox_.getLightCascadeDistances();

    buffer_.cascadesSize       = static_cast<float>(*EngineConf.renderer.shadows.cascadesSize);
    buffer_.cascadesDistance.x = distances[0];
    buffer_.cascadesDistance.y = distances[1];
    buffer_.cascadesDistance.z = distances[2];
    buffer_.cascadesDistance.w = distances[3];
}

void ShadowMapRenderer::cleanUp()
{
    shader_.Clear();
    instancedShader_.Clear();
    entityRenderer_.cleanUp();

    if (perFrameBuffer_)
    {
        context_.graphicsApi_.DeleteShaderBuffer(*perFrameBuffer_);
        perFrameBuffer_ = std::nullopt;
    }

    for (uint32 cascadeIndex = 0; cascadeIndex < Params::MAX_SHADOW_MAP_CASADES; ++cascadeIndex)
    {
        if (shadowFrameBuffer_[cascadeIndex])
        {
            context_.graphicsApi_.DeleteFrameBuffer(*shadowFrameBuffer_[cascadeIndex]);
            shadowFrameBuffer_[cascadeIndex] = nullptr;
        }
    }
}

void ShadowMapRenderer::renderScene()
{
    if (EngineConf.renderer.useInstanceRendering)
    {
        entityRenderer_.renderEntityWithGrouping(shader_, instancedShader_);
    }
    else
    {
        shader_.Start();
        entityRenderer_.renderEntitiesWithoutGrouping();
        shader_.Stop();
    }
}

bool ShadowMapRenderer::isInit() const
{
    return shader_.IsReady() and perFrameBuffer_.has_value();
}

void ShadowMapRenderer::prepare()
{
    if (not isInit() or not isActive_)
        return;

    prepareFrameBuffer();

    uint32 lastBindedPerFrameBuffer = context_.graphicsApi_.BindShaderBuffer(*perFrameBuffer_);

    auto shadowMapSize = *EngineConf.renderer.shadows.mapSize;
    context_.graphicsApi_.SetViewPort(0, 0, shadowMapSize, shadowMapSize);

    context_.graphicsApi_.EnableDepthTest();
    context_.graphicsApi_.DisableCulling();
    renderCascades();
    context_.graphicsApi_.EnableCulling();
    context_.graphicsApi_.DisableDepthTest();

    const auto& renderingSize = context_.projection_.GetRenderingSize();
    context_.graphicsApi_.SetViewPort(0, 0, renderingSize.x, renderingSize.y);
    context_.graphicsApi_.BindShaderBuffer(lastBindedPerFrameBuffer);
}

void ShadowMapRenderer::subscribe(GameObject& gameObject)
{
    entityRenderer_.subscribe(gameObject);
}

void ShadowMapRenderer::unSubscribe(GameObject& gameObject)
{
    entityRenderer_.unSubscribe(gameObject);
}

void ShadowMapRenderer::unSubscribeAll()
{
    entityRenderer_.unSubscribeAll();
}

void ShadowMapRenderer::reloadShaders()
{
    shader_.Reload();
    instancedShader_.Reload();
}

void ShadowMapRenderer::prepareFrameBuffer()
{
    shadowBox_.update(context_.scene_->GetCamera(), context_.scene_->GetDirectionalLight());

    const auto& shadowMatrices = shadowBox_.getLightProjectionViewMatrices();

    for (uint32 cascadeIndex = 0; cascadeIndex < Params::MAX_SHADOW_MAP_CASADES; ++cascadeIndex)
    {
        buffer_.directionalLightSpace[cascadeIndex] =
            context_.graphicsApi_.PrepareMatrixToLoad(convertNdcToTextureCooridates(shadowMatrices[cascadeIndex]));
    }
    context_.graphicsApi_.UpdateShaderBuffer(*context_.shadowsBufferId_, &buffer_);
}

void ShadowMapRenderer::renderCascades()
{
    auto lightMatrixes = shadowBox_.getLightProjectionViewMatrices();
    for (uint32 cascadeIndex = 0; cascadeIndex < Params::MAX_SHADOW_MAP_CASADES; ++cascadeIndex)
    {
        shadowFrameBuffer_[cascadeIndex]->Clear();
        shadowFrameBuffer_[cascadeIndex]->Bind(GraphicsApi::FrameBuffer::BindType::Write);

        PerFrameBuffer perFrame;
        perFrame.ProjectionViewMatrix = context_.graphicsApi_.PrepareMatrixToLoad(lightMatrixes[cascadeIndex]);
        context_.graphicsApi_.UpdateShaderBuffer(*perFrameBuffer_, &perFrame);

        renderScene();
        shadowFrameBuffer_[cascadeIndex]->UnBind();
    }
}

mat4 ShadowMapRenderer::convertNdcToTextureCooridates(const mat4& lightSpaceMatrix) const
{
    return biasMatrix_ * lightSpaceMatrix;
}
}  // namespace GameEngine
