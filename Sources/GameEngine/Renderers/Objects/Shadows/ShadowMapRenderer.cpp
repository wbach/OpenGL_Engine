#include "ShadowMapRenderer.hpp"

#include <GLM/GLMUtils.h>
#include <GraphicsApi/ShaderProgramType.h>
#include <Logger/Log.h>

#include <math.hpp>

#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"

namespace GameEngine
{
namespace
{
std::mutex rendererSubscriberMutex;
}
ShadowMapRenderer::ShadowMapRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Shadows)
    , shadowBox_(context.projection_)
    , projectionViewMatrix_(1.f)
    , biasMatrix_(Utils::CreateBiasNdcToTextureCoordinates())
{
    for (uint32 cascadeIndex = 0; cascadeIndex < Params::MAX_SHADOW_MAP_CASADES; ++cascadeIndex)
    {
        shadowFrameBuffer_[cascadeIndex] = nullptr;
    }
}

ShadowMapRenderer::~ShadowMapRenderer()
{
    if (perFrameBuffer_)
    {
        context_.graphicsApi_.DeleteShaderBuffer(*perFrameBuffer_);
    }

    for (uint32 cascadeIndex = 0; cascadeIndex < Params::MAX_SHADOW_MAP_CASADES; ++cascadeIndex)
    {
        if (shadowFrameBuffer_[cascadeIndex])
        {
            context_.graphicsApi_.DeleteFrameBuffer(*shadowFrameBuffer_[cascadeIndex]);
        }
    }
}

void ShadowMapRenderer::init()
{
    shader_.Init();
    GraphicsApi::FrameBuffer::Attachment depthAttachment(EngineConf.renderer.shadows.mapSize,
                                                         GraphicsApi::FrameBuffer::Type::Depth,
                                                         GraphicsApi::FrameBuffer::Format::Depth);

    depthAttachment.wrapMode    = GraphicsApi::FrameBuffer::WrapMode::ClampToEdge;
    depthAttachment.filter      = GraphicsApi::FrameBuffer::Filter::Linear;
    depthAttachment.compareMode = GraphicsApi::FrameBuffer::CompareMode::RefToTexture;

    perFrameBuffer_ = context_.graphicsApi_.CreateShaderBuffer(PER_FRAME_BIND_LOCATION, sizeof(PerFrameBuffer));

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
            ERROR_LOG("Shadow framebuffer creation error.");
            return;
        }

        context_.cascadedShadowMapsIds_[cascadeIndex] =
            shadowFrameBuffer->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Depth);
    }

    // relevant to shadowbox
    const auto& distances = shadowBox_.getLightCascadeDistances();

    buffer_.cascadesSize = static_cast<float>(EngineConf.renderer.shadows.cascadesSize);
    buffer_.cascadesDistance.x = distances[0];
    buffer_.cascadesDistance.y = distances[1];
    buffer_.cascadesDistance.z = distances[2];
    buffer_.cascadesDistance.w = distances[3];
}

bool ShadowMapRenderer::isInit() const
{
    return shader_.IsReady() and perFrameBuffer_.has_value();
}

void ShadowMapRenderer::prepare()
{
    if (not isInit())
        return;

    prepareFrameBuffer();

    uint32 lastBindedPerFrameBuffer = context_.graphicsApi_.BindShaderBuffer(*perFrameBuffer_);

    auto shadowMapSize = EngineConf.renderer.shadows.mapSize;
    context_.graphicsApi_.SetViewPort(0, 0, shadowMapSize, shadowMapSize);

    shader_.Start();
    context_.graphicsApi_.EnableDepthTest();
    renderCascades();
    context_.graphicsApi_.DisableDepthTest();

    const auto& renderingSize = context_.projection_.GetRenderingSize();
    context_.graphicsApi_.SetViewPort(0, 0, renderingSize.x, renderingSize.y);
    context_.graphicsApi_.BindShaderBuffer(lastBindedPerFrameBuffer);
}

void ShadowMapRenderer::subscribe(GameObject& gameObject)
{
    auto rendererComponent = gameObject.GetComponent<Components::RendererComponent>();

    if (rendererComponent)
    {
        auto animator = gameObject.GetComponent<Components::Animator>();

        std::lock_guard<std::mutex> lk(rendererSubscriberMutex);
        subscribes_.push_back({&gameObject, rendererComponent, animator});
    }
}

void ShadowMapRenderer::unSubscribe(GameObject& gameObject)
{
    std::lock_guard<std::mutex> lk(rendererSubscriberMutex);
    for (auto iter = subscribes_.begin(); iter != subscribes_.end();)
    {
        if ((*iter).gameObject->GetId() == gameObject.GetId())
        {
            iter = subscribes_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void ShadowMapRenderer::unSubscribeAll()
{
    subscribes_.clear();
}

void ShadowMapRenderer::reloadShaders()
{
    shader_.Reload();
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

void ShadowMapRenderer::renderCascades() const
{
    auto lightMatrixes = shadowBox_.getLightProjectionViewMatrices();
    for (uint32 cascadeIndex = 0; cascadeIndex < Params::MAX_SHADOW_MAP_CASADES; ++cascadeIndex)
    {
        shadowFrameBuffer_[cascadeIndex]->Clear();
        shadowFrameBuffer_[cascadeIndex]->Bind(GraphicsApi::FrameBuffer::BindType::Write);

        PerFrameBuffer perFrame;
        perFrame.ProjectionViewMatrix = context_.graphicsApi_.PrepareMatrixToLoad(lightMatrixes[cascadeIndex]);
        context_.graphicsApi_.UpdateShaderBuffer(*perFrameBuffer_, &perFrame);
        renderSubscribes();
        shadowFrameBuffer_[cascadeIndex]->UnBind();
    }
}

void ShadowMapRenderer::renderSubscribes() const
{
    std::lock_guard<std::mutex> lk(rendererSubscriberMutex);
    for (auto& sub : subscribes_)
        renderSubscriber(sub);
}

void ShadowMapRenderer::renderSubscriber(const ShadowMapSubscriber& sub) const
{
    auto model = sub.renderComponent->GetModelWrapper().Get(LevelOfDetail::L1);

    if (not model)
        return;

    if (sub.animator and model->getRootJoint())
    {
        const auto& perPoseBuffer = sub.animator->getPerPoseBufferId();

        if (perPoseBuffer)
            context_.graphicsApi_.BindShaderBuffer(*perPoseBuffer);
    }

    const auto& meshes = model->GetMeshes();

    uint32 meshId = 0;
    for (const auto& mesh : meshes)
    {
        if (not mesh.GetGraphicsObjectId())
            continue;

        const auto& meshBuffer = mesh.getShaderBufferId();

        if (meshBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*meshBuffer);
        }

        const auto& perMeshUpdateBuffer = sub.renderComponent->GetPerObjectUpdateBuffer(meshId);
        if (perMeshUpdateBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*perMeshUpdateBuffer);
        }

        const auto& perMeshConstantBuffer = sub.renderComponent->GetPerObjectConstantsBuffer(meshId);
        if (perMeshConstantBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*perMeshConstantBuffer);
        }

        ++meshId;
        renderMesh(mesh);
    }
}

void ShadowMapRenderer::renderMesh(const Mesh& mesh) const
{
    const auto& material = mesh.GetMaterial();

    if (material.diffuseTexture)
        context_.graphicsApi_.ActiveTexture(0, *material.diffuseTexture->GetGraphicsObjectId());

    context_.graphicsApi_.RenderMesh(*mesh.GetGraphicsObjectId());
}
mat4 ShadowMapRenderer::convertNdcToTextureCooridates(const mat4& lightSpaceMatrix) const
{
    return biasMatrix_ * lightSpaceMatrix;
}
}  // namespace GameEngine
