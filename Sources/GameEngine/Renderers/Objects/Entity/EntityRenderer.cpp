#include "EntityRenderer.h"

#include <Logger/Log.h>

#include <algorithm>

#include "GameEngine/Components/Animation/Animator.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
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
    cleanUp();
}

uint32 EntityRenderer::renderEntitiesWithoutGrouping()
{
    renderedMeshes_ = 0;

    std::lock_guard<std::mutex> lk(subscriberMutex_);
    if (not subscribes_.empty())
    {
        for (const auto& sub : subscribes_)
        {
            const auto& objectTransform = sub.gameObject->GetWorldTransform();
            auto radius                 = glm::compMax(objectTransform.GetScale());
            auto isVisible              = context_.frustrum_.intersection(objectTransform.GetPosition(), radius);

            if (isVisible or true) // TO DO : fix
            {
                auto distance = context_.scene_->distanceToCamera(*sub.gameObject);
                if (auto model = sub.renderComponent->GetModelWrapper().get(distance))
                {
                    renderModel(sub, *model);
                }
            }
        }
    }
    return renderedMeshes_;
}

void EntityRenderer::init()
{
    perInstanceBuffer_ =
        std::make_unique<BufferObject<PerInstances>>(context_.graphicsApi_, PER_INSTANCES_BIND_LOCATION);
    perInstanceBuffer_->GpuLoadingPass();

    perMeshBuffer_ =
        std::make_unique<BufferObject<PerObjectUpdate>>(context_.graphicsApi_, PER_OBJECT_UPDATE_BIND_LOCATION);
    perMeshBuffer_->GpuLoadingPass();
}

void EntityRenderer::cleanUp()
{
    if (perInstanceBuffer_)
    {
        context_.gpuLoader_.AddObjectToRelease(std::move(perInstanceBuffer_));
    }
    if (perMeshBuffer_)
    {
        context_.gpuLoader_.AddObjectToRelease(std::move(perMeshBuffer_));
    }
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
    DEBUG_LOG("subscribes_ size: " + std::to_string(subscribes_.size()) +  " subscribes_ clear");
    subscribes_.clear();
}

uint32 EntityRenderer::renderEntityWithGrouping(ShaderProgram& singleEntityShader, ShaderProgram& instancedEntityShader)
{
    renderedMeshes_ = 0;
    std::lock_guard<std::mutex> lk(subscriberMutex_);

    auto groupedEntities = groupEntities();

    if (not groupedEntities.singleEntitiesToRender_.empty())
    {
        singleEntityShader.Start();
        for (const auto& [model, subscribers] : groupedEntities.singleEntitiesToRender_)
        {
            for (auto& subscriber : subscribers)
                renderModel(*subscriber, *model);
        }
    }

    if (not groupedEntities.groupsToRender_.empty())
    {
        instancedEntityShader.Start();
        for (const auto& [model, group] : groupedEntities.groupsToRender_)
        {
            for (auto& subscribers : group)
            {
                const auto perInstancesApiId = perInstanceBuffer_->GetGraphicsObjectId();
                const auto perMeshApiId      = perMeshBuffer_->GetGraphicsObjectId();

                if (perInstancesApiId and perMeshApiId)
                {
                    auto& data = perInstanceBuffer_->GetData();
                    context_.graphicsApi_.BindShaderBuffer(*perInstancesApiId);

                    for (size_t i = 0; i < subscribers.size(); ++i)
                    {
                        auto& sub          = subscribers[i];
                        data.transforms[i] = context_.graphicsApi_.PrepareMatrixToLoad(
                            sub->gameObject->GetWorldTransform().CalculateCurrentMatrix());
                    }
                    perInstanceBuffer_->UpdateGpuPass();

                    for (const auto& mesh : model->GetMeshes())
                    {
                        if (not mesh.GetGraphicsObjectId())
                            continue;

                        if (const auto& meshBuffer = mesh.getShaderBufferId())
                        {
                            context_.graphicsApi_.BindShaderBuffer(*meshBuffer);
                        }

                        perMeshBuffer_->GetData().TransformationMatrix =
                            context_.graphicsApi_.PrepareMatrixToLoad(mesh.GetMeshTransform());
                        perMeshBuffer_->UpdateGpuPass();
                        context_.graphicsApi_.BindShaderBuffer(*perMeshApiId);

                        bindMaterial(mesh.GetMaterial());
                        context_.graphicsApi_.RenderMeshInstanced(*mesh.GetGraphicsObjectId(),
                                                                  static_cast<uint32>(subscribers.size()));
                        unBindMaterial(mesh.GetMaterial());
                        renderedMeshes_ += static_cast<uint32>(subscribers.size());
                    }
                }
            }
        }
    }

    if (not groupedEntities.singleEntitiesToRender_.empty() or not groupedEntities.groupsToRender_.empty())
    {
        instancedEntityShader.Stop();
    }

    return renderedMeshes_;
}

EntityRenderer::GroupedEntities EntityRenderer::groupEntities() const
{
    GroupedEntities result;

    for (const auto& sub : subscribes_)
    {
        const auto& objectTransform = sub.gameObject->GetWorldTransform();
        auto radius                 = glm::compMax(objectTransform.GetScale());
        auto isVisible              = context_.frustrum_.intersection(objectTransform.GetPosition(), radius);

        if (not isVisible)
            continue;

        auto distance = context_.scene_->distanceToCamera(*sub.gameObject);

        if (auto model = sub.renderComponent->GetModelWrapper().get(distance))
        {
            if ((sub.animator and model->getRootJoint()))
            {
                auto classificatedToSingleIter = result.singleEntitiesToRender_.find(model);
                if (classificatedToSingleIter != result.singleEntitiesToRender_.end())
                {
                    classificatedToSingleIter->second.push_back(&sub);
                }
                else
                {
                    result.singleEntitiesToRender_.insert({model, {&sub}});
                }
            }
            else
            {
                auto classificatedToSingleIter = result.singleEntitiesToRender_.find(model);
                if (classificatedToSingleIter != result.singleEntitiesToRender_.end())
                {
                    if (classificatedToSingleIter->second.size() > 1)
                    {
                        ERROR_LOG("Multiple single should be only for animated models");
                        continue;
                    }

                    result.groupsToRender_.insert(
                        {classificatedToSingleIter->first, {{classificatedToSingleIter->second.front(), &sub}}});
                    result.singleEntitiesToRender_.erase(classificatedToSingleIter);
                }
                else
                {
                    auto iter = result.groupsToRender_.find(model);
                    if (iter != result.groupsToRender_.end())
                    {
                        if (iter->second.back().size() >= MAX_INSTANCES)
                        {
                            iter->second.push_back({&sub});
                        }
                        else
                        {
                            iter->second.back().push_back(&sub);
                        }
                    }
                    else
                    {
                        result.singleEntitiesToRender_.insert({model, {&sub}});
                    }
                }
            }
        }
    }
    return result;
}

void EntityRenderer::renderModel(const EntitySubscriber& subsriber, const Model& model)
{
    if (subsriber.animator and model.getRootJoint())
    {
        const auto& perPoseBuffer = subsriber.animator->getPerPoseBufferId();

        if (perPoseBuffer)
            context_.graphicsApi_.BindShaderBuffer(*perPoseBuffer);
    }

    const auto& meshes = model.GetMeshes();

    for (const auto& mesh : meshes)
    {
        if (not mesh.GetGraphicsObjectId())
        {
            DEBUG_LOG("not mesh.GetGraphicsObjectId()");
            continue;
        }

        const auto& meshBuffer = mesh.getShaderBufferId();

        if (meshBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*meshBuffer);
        }
        else
        {
            DEBUG_LOG("not meshBuffer");
            continue;
        }

        const auto& perMeshUpdateBuffer = subsriber.renderComponent->GetPerObjectUpdateBuffer(mesh.GetGpuObjectId());
        if (perMeshUpdateBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*perMeshUpdateBuffer);
        }
        else
        {
            DEBUG_LOG("not perMeshUpdateBuffer");
            continue;
        }

        const auto& perMeshConstantBuffer =
            subsriber.renderComponent->GetPerObjectConstantsBuffer(mesh.GetGpuObjectId());
        if (perMeshConstantBuffer)
        {
            context_.graphicsApi_.BindShaderBuffer(*perMeshConstantBuffer);
        }
        else
        {
            DEBUG_LOG("not perMeshConstantBuffer");
            continue;
        }

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
