#include "SkyBoxRenderer.h"

#include <GraphicsApi/ShaderProgramType.h>
#include <Logger/Log.h>

#include "GameEngine/Components/Renderer/SkyBox/SkyBoxComponent.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
SkyBoxRenderer::SkyBoxRenderer(RendererContext& context)
    : context_(context)
    , rotationSpeed_(1.f)
    , rotation_(0.f)
    , scale_(150.f)
{
    viewDistanceChangeSubscription_ =
        EngineConf.renderer.viewDistance.subscribeForChange([this](const auto&) { calculateBoxScale(); });
}

SkyBoxRenderer::~SkyBoxRenderer()
{
    cleanUp();
    EngineConf.renderer.viewDistance.unsubscribe(viewDistanceChangeSubscription_);
}

void SkyBoxRenderer::init()
{
    if (not perObjectUpdateId_)
    {
        perObjectUpdateId_ =
            context_.graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));
    }
    if (not perMeshObjectId_)
    {
        perMeshObjectId_ = context_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION,
                                                                    sizeof(SkyBoxRenderer::PerMeshObject));

        perMeshObject_.blendFactor_ = 1.f;
        perMeshObject_.fogColor_    = context_.fogColor_;
    }
    // max size : skybox width <= (2 * sqrt(3) / 3)
    calculateBoxScale();
}

void SkyBoxRenderer::cleanUp()
{
    if (perObjectUpdateId_)
    {
        context_.graphicsApi_.DeleteShaderBuffer(*perObjectUpdateId_);
        perObjectUpdateId_ = std::nullopt;
    }
    if (perMeshObjectId_)
    {
        context_.graphicsApi_.DeleteShaderBuffer(*perMeshObjectId_);
        perMeshObjectId_ = std::nullopt;
    }
}

void SkyBoxRenderer::prepareToRendering()
{
    updateBuffer();
    prepareShaderBeforeFrameRender();
}

void SkyBoxRenderer::updateBuffer()
{
    rotation_.y += context_.time_.deltaTime * rotationSpeed_;

    auto position = context_.scene_ ? context_.scene_->GetCamera().GetPosition() : vec3(0);

    perObjectUpdateBuffer_.TransformationMatrix =
        Utils::CreateTransformationMatrix(position, DegreesVec3(rotation_), scale_);

    context_.graphicsApi_.UpdateShaderBuffer(*perObjectUpdateId_, &perObjectUpdateBuffer_);
    context_.graphicsApi_.BindShaderBuffer(*perObjectUpdateId_);
}

void SkyBoxRenderer::calculateBoxScale()
{
    scale_ = vec3(context_.projection_.GetViewDistance() * sqrtf(3) / 3.f);
}

void SkyBoxRenderer::unSubscribeAll()
{
    std::lock_guard<std::mutex> lk(subscriberMutex_);
    subscribes_.clear();
}

void SkyBoxRenderer::render()
{
    if (subscribes_.empty())
        return;

    prepareToRendering();

    std::lock_guard<std::mutex> lk(subscriberMutex_);
    for (const auto& subscriber : subscribes_)
    {
        RenderSkyBoxModel(subscriber.second);
    }
}

void SkyBoxRenderer::prepareShaderBeforeFrameRender()
{
    perMeshObject_.blendFactor_ = context_.scene_ ? context_.scene_->GetDayNightCycle().GetDayNightBlendFactor() : 1.f;
    context_.graphicsApi_.UpdateShaderBuffer(*perMeshObjectId_, &perMeshObject_);
    context_.graphicsApi_.BindShaderBuffer(*perMeshObjectId_);
}

void SkyBoxRenderer::RenderSkyBoxModel(const SkyBoxSubscriber& sub)
{
    const auto& meshes = sub.model_.GetMeshes();

    BindTextures(sub);

    for (const auto& mesh : meshes)
    {
        RenderSkyBoxMesh(mesh);
    }
}

void SkyBoxRenderer::subscribe(GameObject& gameObject)
{
    std::lock_guard<std::mutex> lk(subscriberMutex_);
    auto component = gameObject.GetComponent<Components::SkyBoxComponent>();

    if (component)
    {
        subscribes_.insert(
            {gameObject.GetId(), {*component->GetModel(), component->GetDayTexture(), component->GetNightTexture()}});
    }
}

void SkyBoxRenderer::unSubscribe(GameObject& gameObject)
{
    std::lock_guard<std::mutex> lk(subscriberMutex_);
    auto iter = subscribes_.find(gameObject.GetId());

    if (iter != subscribes_.end())
    {
        subscribes_.erase(iter);
    }
}

void SkyBoxRenderer::BindTextures(const SkyBoxSubscriber& sub) const
{
    BindCubeMapTexture(*sub.dayTexture_, 0);
    BindCubeMapTexture(*sub.nightTexture_, 1);
}

void SkyBoxRenderer::BindCubeMapTexture(const Texture& texture, uint32 id) const
{
    if (texture.GetGraphicsObjectId())
        context_.graphicsApi_.ActiveTexture(id, *texture.GetGraphicsObjectId());
}

void SkyBoxRenderer::RenderSkyBoxMesh(const Mesh& mesh) const
{
    if (not mesh.GetGraphicsObjectId())
        return;

    context_.graphicsApi_.RenderMesh(*mesh.GetGraphicsObjectId());
}
}  // namespace GameEngine
