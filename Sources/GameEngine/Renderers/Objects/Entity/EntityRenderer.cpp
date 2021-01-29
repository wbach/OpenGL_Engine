#include "EntityRenderer.h"

#include <Logger/Log.h>
#include <algorithm>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "GraphicsApi/ShaderProgramType.h"

namespace GameEngine
{
EntityRenderer::EntityRenderer(RendererContext& context)
    : context_(context)
{
}

EntityRenderer::~EntityRenderer()
{
    DEBUG_LOG("");
    unSubscribeAll();
}

uint32 EntityRenderer::render()
{
    renderedMeshes_ = 0;

    if (not subscribes_.empty())
    {
        renderEntities();
    }

    return renderedMeshes_;
}

void EntityRenderer::subscribe(GameObject& gameObject)
{
    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                             [id = gameObject.GetId()](const auto& sub) { return sub.gameObject->GetId() == id; });

    if (iter != subscribes_.end())
        return;

    auto rendererComponent = gameObject.GetComponent<Components::RendererComponent>();

    if (rendererComponent == nullptr)
        return;

    auto model = rendererComponent->GetModelWrapper().Get(LevelOfDetail::L1);
    if (not model)
        return;

    auto animator = gameObject.GetComponent<Components::Animator>();

    std::lock_guard<std::mutex> lk(subscriberMutex_);
    subscribes_.push_back({&gameObject, rendererComponent, animator});
    subscribesIds_.insert(gameObject.GetId());
}

void EntityRenderer::unSubscribe(GameObject& gameObject)
{
    if (not subscribesIds_.count(gameObject.GetId()))
    {
        return;
    }
    std::lock_guard<std::mutex> lk(subscriberMutex_);
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
    subscribesIds_.erase(gameObject.GetId());
}

void EntityRenderer::unSubscribeAll()
{
    subscribes_.clear();
}

void EntityRenderer::renderEntities()
{
    std::lock_guard<std::mutex> lk(subscriberMutex_);

    for (const auto& sub : subscribes_)
    {
        auto model = sub.renderComponent->GetModelWrapper().Get(LevelOfDetail::L1);

        if (model)
        {
            renderModel(sub, *model);
        }
    }
}

void EntityRenderer::renderModel(const EntitySubscriber& subsriber, const Model& model)
{
    auto radius = glm::compMax(subsriber.gameObject->GetWorldTransform().GetScale());
    auto isVisible = context_.frustrum_.intersection(subsriber.gameObject->GetWorldTransform().GetPosition(), radius);

    if (not isVisible)
        return;

    if (subsriber.animator and model.getRootJoint())
    {
        const auto& perPoseBuffer = subsriber.animator->getPerPoseBufferId();

        if (perPoseBuffer)
            context_.graphicsApi_.BindShaderBuffer(*perPoseBuffer);
    }

    const auto& meshes = model.GetMeshes();

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

        const auto& perMeshUpdateBuffer = subsriber.renderComponent->GetPerObjectUpdateBuffer(meshId);
        if (perMeshUpdateBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*perMeshUpdateBuffer);
        }

        const auto& perMeshConstantBuffer = subsriber.renderComponent->GetPerObjectConstantsBuffer(meshId);
        if (perMeshConstantBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*perMeshConstantBuffer);
        }
        ++meshId;
        renderMesh(mesh);
    }
}

void EntityRenderer::renderMesh(const Mesh& mesh)
{
    ++renderedMeshes_;
    bindMaterial(mesh.GetMaterial());
    context_.graphicsApi_.RenderMesh(*mesh.GetGraphicsObjectId());
    unBindMaterial(mesh.GetMaterial());
}

void EntityRenderer::bindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.DisableCulling();

    const auto& config = EngineConf.renderer.textures;
    bindMaterialTexture(0, material.diffuseTexture, config.useDiffuse);
    bindMaterialTexture(1, material.ambientTexture, config.useAmbient);
    bindMaterialTexture(2, material.normalTexture, config.useNormal);
    bindMaterialTexture(3, material.specularTexture, config.useSpecular);
}

void EntityRenderer::unBindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.EnableCulling();
}

void EntityRenderer::bindMaterialTexture(uint32 location, Texture* texture, bool enabled) const
{
    if (enabled and texture and texture->GetGraphicsObjectId())
    {
        context_.graphicsApi_.ActiveTexture(location, *texture->GetGraphicsObjectId());
    }
}
}  // namespace GameEngine
