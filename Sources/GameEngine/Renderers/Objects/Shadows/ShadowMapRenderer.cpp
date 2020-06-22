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
    , viewOffset_(Utils::CreateOffset())
    , shadowFrameBuffer_(nullptr)
{
}

ShadowMapRenderer::~ShadowMapRenderer()
{
    if (perFrameBuffer_)
    {
        context_.graphicsApi_.DeleteShaderBuffer(*perFrameBuffer_);
    }
    if (shadowFrameBuffer_)
    {
        context_.graphicsApi_.DeleteFrameBuffer(*shadowFrameBuffer_);
    }
}

void ShadowMapRenderer::init()
{
    shader_.Init();
    GraphicsApi::FrameBuffer::Attachment depthAttachment(EngineConf.renderer.shadows.mapSize,
                                                         GraphicsApi::FrameBuffer::Type::Depth,
                                                         GraphicsApi::FrameBuffer::Format::Depth);

    perFrameBuffer_    = context_.graphicsApi_.CreateShaderBuffer(PER_FRAME_BIND_LOCATION, sizeof(PerFrameBuffer));
    shadowFrameBuffer_ = &context_.graphicsApi_.CreateFrameBuffer({depthAttachment});

    auto status = shadowFrameBuffer_->Init();

    if (not status)
    {
        context_.graphicsApi_.DeleteFrameBuffer(*shadowFrameBuffer_);
        shadowFrameBuffer_ = nullptr;
        ERROR_LOG("Shadow framebuffer creation error.");
        return;
    }

    context_.shadowMapId_ = shadowFrameBuffer_->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Depth);
}

bool ShadowMapRenderer::IsInit() const
{
    return shader_.IsReady() and perFrameBuffer_.has_value();
}

void ShadowMapRenderer::prepare()
{
    if (not IsInit() and shadowFrameBuffer_)
        return;

    prepareFrameBuffer();

    uint32 lastBindedPerFrameBuffer = context_.graphicsApi_.BindShaderBuffer(*perFrameBuffer_);
    shadowFrameBuffer_->Clear();
    shadowFrameBuffer_->Bind(GraphicsApi::FrameBuffer::BindType::Write);

    auto shadowMapSize = EngineConf.renderer.shadows.mapSize;
    context_.graphicsApi_.SetViewPort(0, 0, shadowMapSize, shadowMapSize);

    shader_.Start();
    context_.graphicsApi_.EnableDepthTest();
    context_.graphicsApi_.EnableDepthMask();
    RenderSubscribes();
    context_.graphicsApi_.DisableDepthMask();
    context_.graphicsApi_.DisableDepthTest();

    shadowFrameBuffer_->UnBind();

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
    shadowBox_.Update(context_.scene_->GetCamera(), context_.scene_->GetDirectionalLight());

    PerFrameBuffer perFrame;
    perFrame.ProjectionViewMatrix =
        context_.graphicsApi_.PrepareMatrixToLoad(shadowBox_.GetLightProjectionViewMatrix());
    context_.graphicsApi_.UpdateShaderBuffer(*perFrameBuffer_, &perFrame);
    context_.toShadowMapZeroMatrix_ = shadowBox_.GetLightProjectionViewMatrix();
}

void ShadowMapRenderer::RenderSubscribes() const
{
    std::lock_guard<std::mutex> lk(rendererSubscriberMutex);
    for (auto& sub : subscribes_)
        RenderSubscriber(sub);
}

void ShadowMapRenderer::RenderSubscriber(const ShadowMapSubscriber& sub) const
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
        RenderMesh(mesh);
    }
  //  DEBUG_LOG("gameObject " + std::to_string(sub.gameObject->GetWorldTransform().GetMatrix()));
}

void ShadowMapRenderer::RenderMesh(const Mesh& mesh) const
{
    const auto& material = mesh.GetMaterial();

    if (material.diffuseTexture)
        context_.graphicsApi_.ActiveTexture(0, *material.diffuseTexture->GetGraphicsObjectId());

    context_.graphicsApi_.RenderMesh(*mesh.GetGraphicsObjectId());

  //  DEBUG_LOG("mesh " + std::to_string(mesh.GetMeshTransform()));
}

}  // namespace GameEngine
