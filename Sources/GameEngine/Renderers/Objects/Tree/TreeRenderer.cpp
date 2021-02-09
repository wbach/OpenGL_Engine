#include "TreeRenderer.h"
#include <algorithm>
#include <Common/Transform.h>
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/PerInstances.h"
#include "GameEngine/Resources/ShaderBuffers/PerMeshObject.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Engine/Configuration.h"

namespace GameEngine
{
TreeRenderer::TreeRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Tree)
{
}

void TreeRenderer::init()
{
    shader_.Init();
}

void TreeRenderer::render()
{
    if (subscribes_.empty())
        return;

    shader_.Start();

    for (auto& sub : subscribes_)
    {
        auto perInstance = sub.treeRendererComponent_->GetPerInstancesBufferId();
        if (perInstance)
        {
            context_.graphicsApi_.BindShaderBuffer(*perInstance);
        }

        auto perUpdate = sub.treeRendererComponent_->GetPerObjectUpdateId();
        if (perUpdate)
        {
            context_.graphicsApi_.BindShaderBuffer(*perUpdate);
        }

        context_.graphicsApi_.DisableCulling();

        RenderModel(*sub.treeRendererComponent_->GetTopModelWrapper().Get(LevelOfDetail::L1), sub);

        context_.graphicsApi_.EnableCulling();

        RenderModel(*sub.treeRendererComponent_->GetBottomModelWrapper().Get(LevelOfDetail::L1), sub);
    }
}  // namespace GameEngine
void TreeRenderer::subscribe(GameObject& gameObject)
{
    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(), [&gameObject](const TreeSubscriber& sub) {
        return gameObject.GetId() == sub.gameObject_->GetId();
    });

    if (iter == subscribes_.end())
    {
        auto component = gameObject.GetComponent<Components::TreeRendererComponent>();

        if (not component)
            return;

        subscribes_.push_back({&gameObject, component});
    }
}
void TreeRenderer::unSubscribe(GameObject& gameObject)
{
    auto sub = std::find_if(subscribes_.begin(), subscribes_.end(), [&gameObject](const TreeSubscriber& sub) {
        return gameObject.GetId() == sub.gameObject_->GetId();
    });

    if (sub != subscribes_.end())
    {
        subscribes_.erase(sub);
    }
}
void TreeRenderer::unSubscribeAll()
{
    subscribes_.clear();
}
void TreeRenderer::reloadShaders()
{
    shader_.Reload();
}

void TreeRenderer::RenderModel(const Model& model, const TreeSubscriber& sub) const
{
    for (const auto& mesh : model.GetMeshes())
    {
        if (mesh.GetGraphicsObjectId())
        {
            const auto& buffer = mesh.getShaderBufferId();
            context_.graphicsApi_.BindShaderBuffer(*buffer);
            RenderMesh(mesh, sub.treeRendererComponent_->GetInstancesSize());
        }
    }
}
void TreeRenderer::RenderMesh(const Mesh& mesh, uint32 size) const
{
    BindMaterial(mesh.GetMaterial());
    context_.graphicsApi_.RenderMeshInstanced(*mesh.GetGraphicsObjectId(), size);
}
void TreeRenderer::RenderTrees()
{
}
void TreeRenderer::BindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.DisableCulling();

    const auto& config = EngineConf.renderer.textures;
    BindMaterialTexture(1, material.diffuseTexture, config.useDiffuse);
    BindMaterialTexture(2, material.ambientTexture, config.useAmbient);
    BindMaterialTexture(3, material.normalTexture, config.useNormal);
    BindMaterialTexture(4, material.specularTexture, config.useSpecular);
}
void TreeRenderer::UnBindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.EnableCulling();
}
void TreeRenderer::BindMaterialTexture(uint32 location, Texture* texture, bool enabled) const
{
    if (enabled and texture and texture->GetGraphicsObjectId())
    {
        context_.graphicsApi_.ActiveTexture(location, *texture->GetGraphicsObjectId());
    }
}
}  // namespace GameEngine
