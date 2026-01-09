#include "ShadowMapRenderer.hpp"

#include <GLM/GLMUtils.h>
#include <GraphicsApi/ShaderProgramType.h>
#include <Logger/Log.h>
#include <Utils/Time/Timer.h>

#include <math.hpp>

#include "GameEngine/Components/Lights/DirectionalLightComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Objects/Tree/TreeRenderer.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"
#include "magic_enum/magic_enum.hpp"

namespace GameEngine
{
ShadowMapRenderer::ShadowMapRenderer(RendererContext& context)
    : context_(context)
    , entityRenderer_(context)
    , treeRenderer_(context, GraphicsApi::ShaderProgramType::TreeLeafs, GraphicsApi::ShaderProgramType::Entity) // To do use simple tree shaders
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Shadows)
    , instancedShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::InstancesShadows)
    , shadowBox_()
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

    measurementValue_ = &context.measurmentHandler_.AddNewMeasurment("ShadowMapRendererdMeshes", "0");
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
    treeRenderer_.init();

    GraphicsApi::FrameBuffer::Attachment depthAttachment(vec2ui(*EngineConf.renderer.shadows.mapSize),
                                                         GraphicsApi::FrameBuffer::Type::Depth,
                                                         GraphicsApi::FrameBuffer::Format::Depth);

    depthAttachment.wrapMode    = GraphicsApi::FrameBuffer::WrapMode::ClampToEdge;
    depthAttachment.filter      = GraphicsApi::FrameBuffer::Filter::Linear;
    depthAttachment.compareMode = GraphicsApi::FrameBuffer::CompareMode::RefToTexture;

    perFrameBuffer_ =
        context_.graphicsApi_.CreateShaderBuffer(PER_FRAME_BIND_LOCATION, sizeof(PerFrameBuffer), GraphicsApi::DrawFlag::Dynamic);

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

        if (not shadowsBufferId_)
            shadowsBufferId_ = context_.graphicsApi_.CreateShaderBuffer(SHADOW_BUFFER_BIND_LOCATION, sizeof(ShadowsBuffer),
                                                                        GraphicsApi::DrawFlag::Dynamic);
        if (shadowsBufferId_)
            context_.graphicsApi_.BindShaderBuffer(*shadowsBufferId_);

        auto status = shadowFrameBuffer->Init();

        if (not status)
        {
            context_.graphicsApi_.DeleteFrameBuffer(*shadowFrameBuffer);
            shadowFrameBuffer = nullptr;
            LOG_ERROR << "Shadow framebuffer creation error.";
            shader_.Clear();
            return;
        }

        auto shadowmapId = shadowFrameBuffer->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Depth);
        context_.sharedTextures[magic_enum::enum_index(SharedTextures::shadowCascade0).value() + cascadeIndex] = shadowmapId;
        LOG_DEBUG << "Shadow map casade : " << cascadeIndex << ", glid = " << shadowmapId;
    }

    // relevant to shadowbox
    const auto& distances = shadowBox_.getLightCascadeDistances();

    buffer_.cascadesSize     = static_cast<float>(*EngineConf.renderer.shadows.cascadesSize);
    buffer_.cascadesDistance = vec4(distances[0], distances[1], distances[2], distances[3]);
}

void ShadowMapRenderer::cleanUp()
{
    shader_.Clear();
    instancedShader_.Clear();
    entityRenderer_.cleanUp();
    treeRenderer_.cleanUp();

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
        rendererdMeshesCounter_ += entityRenderer_.renderEntityWithGrouping(shader_, instancedShader_);
    }
    else
    {
        shader_.Start();
        rendererdMeshesCounter_ += entityRenderer_.renderEntitiesWithoutGrouping();
        //shader_.Stop();
        treeRenderer_.render();
    }
}

bool ShadowMapRenderer::isInit() const
{
    return shader_.IsReady() and perFrameBuffer_.has_value();
}

void ShadowMapRenderer::prepare()
{
    if (not isInit() or not isActive_)
    {
        measurementValue_->SetValue("0");
        return;
    }

    Utils::Timer timer;

    if (not prepareFrameBuffer())
    {
        measurementValue_->SetValue(std::to_string(rendererdMeshesCounter_));
        return;
    }

    rendererdMeshesCounter_         = 0;
    uint32 lastBindedPerFrameBuffer = context_.graphicsApi_.BindShaderBuffer(*perFrameBuffer_);

    auto shadowMapSize = *EngineConf.renderer.shadows.mapSize;
    context_.graphicsApi_.SetViewPort(0, 0, shadowMapSize, shadowMapSize);

    context_.graphicsApi_.EnableDepthTest();
    context_.graphicsApi_.DisableCulling();
    renderCascades();
    context_.graphicsApi_.EnableCulling();
    context_.graphicsApi_.DisableDepthTest();

    const auto& renderingSize = context_.camera_->GetProjection().GetRenderingSize();
    context_.graphicsApi_.SetViewPort(0, 0, renderingSize.x, renderingSize.y);
    context_.graphicsApi_.BindShaderBuffer(lastBindedPerFrameBuffer);

    measurementValue_->SetValue(std::to_string(rendererdMeshesCounter_));
}

void ShadowMapRenderer::subscribe(GameObject& gameObject)
{
    entityRenderer_.subscribe(gameObject);
    treeRenderer_.subscribe(gameObject);
}

void ShadowMapRenderer::unSubscribe(GameObject& gameObject)
{
    entityRenderer_.unSubscribe(gameObject);
    treeRenderer_.unSubscribe(gameObject);
}

void ShadowMapRenderer::unSubscribeAll()
{
    entityRenderer_.unSubscribeAll();
    treeRenderer_.unSubscribeAll();
}

void ShadowMapRenderer::reloadShaders()
{
    shader_.Reload();
    instancedShader_.Reload();
    treeRenderer_.reloadShaders();
}

bool ShadowMapRenderer::prepareFrameBuffer()
{
    auto directionalLights =
        context_.scene_->getComponentController().GetAllActiveComponentsOfType<Components::DirectionalLightComponent>();

    if (directionalLights.empty())
    {
        return false;
    }

    const auto& directionalLight = directionalLights.front();
    shadowBox_.update(*context_.camera_, *directionalLight);

    const auto& shadowMatrices = shadowBox_.getLightProjectionViewMatrices();

    for (uint32 cascadeIndex = 0; cascadeIndex < Params::MAX_SHADOW_MAP_CASADES; ++cascadeIndex)
    {
        buffer_.directionalLightSpace[cascadeIndex] =
            context_.graphicsApi_.PrepareMatrixToLoad(convertNdcToTextureCooridates(shadowMatrices[cascadeIndex]));
    }
    context_.graphicsApi_.UpdateShaderBuffer(*shadowsBufferId_, &buffer_);
    return true;
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
        perFrame.cameraPosition       = context_.camera_->GetPosition();
        perFrame.projection           = context_.camera_->GetProjection().GetBufferParams();
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
