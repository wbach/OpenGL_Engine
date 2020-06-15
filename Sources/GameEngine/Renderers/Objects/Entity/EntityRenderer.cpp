#include "EntityRenderer.h"

#include <Logger/Log.h>

#include <Mutex.hpp>
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
namespace
{
std::mutex entityRendererSubscriberMutex;
}
EntityRenderer::EntityRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Entity)
{
}

EntityRenderer::~EntityRenderer()
{
    DEBUG_LOG("");
    unSubscribeAll();
}

void EntityRenderer::init()
{
    DEBUG_LOG("");
    shader_.Init();
}

void EntityRenderer::render()
{
    if (not shader_.IsReady() or subscribes_.empty())
        return;

    shader_.Start();
    RenderEntities();
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

    std::lock_guard<std::mutex> lk(entityRendererSubscriberMutex);
    subscribes_.push_back({&gameObject, rendererComponent, animator});
    subscribesIds_.insert(gameObject.GetId());
}

void EntityRenderer::unSubscribe(GameObject& gameObject)
{
    if (not subscribesIds_.count(gameObject.GetId()))
    {
        return;
    }

    std::lock_guard<std::mutex> lk(entityRendererSubscriberMutex);

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

void EntityRenderer::reloadShaders()
{
    DEBUG_LOG("");
    shader_.Reload();
}

void EntityRenderer::RenderEntities()
{
    std::lock_guard<std::mutex> lk(entityRendererSubscriberMutex);

    for (const auto& sub : subscribes_)
    {
        auto model = sub.renderComponent->GetModelWrapper().Get(LevelOfDetail::L1);

        if (not model)
            continue;

        RenderModel(sub, *model);
    }
}

void EntityRenderer::RenderModel(const EntitySubscriber& subsriber, const Model& model) const
{
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
        RenderMesh(mesh);
    }
}

void EntityRenderer::RenderMesh(const Mesh& mesh) const
{
    BindMaterial(mesh.GetMaterial());
    context_.graphicsApi_.RenderMesh(*mesh.GetGraphicsObjectId());
    UnBindMaterial(mesh.GetMaterial());
}

void EntityRenderer::BindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.DisableCulling();

    const auto& config = EngineConf.renderer.textures;
    BindMaterialTexture(0, material.diffuseTexture, config.useDiffuse);
    BindMaterialTexture(1, material.ambientTexture, config.useAmbient);
    BindMaterialTexture(2, material.normalTexture, config.useNormal);
    BindMaterialTexture(3, material.specularTexture, config.useSpecular);
}

void EntityRenderer::UnBindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.EnableCulling();
}

void EntityRenderer::BindMaterialTexture(uint32 location, Texture* texture, bool enabled) const
{
    if (enabled and texture and texture->GetGraphicsObjectId())
    {
        context_.graphicsApi_.ActiveTexture(location, *texture->GetGraphicsObjectId());
    }
}
}  // namespace GameEngine
