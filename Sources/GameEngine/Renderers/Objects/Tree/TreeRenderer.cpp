#include "TreeRenderer.h"

#include <Common/Transform.h>

#include <algorithm>

#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/PerMeshObject.h"
#include "GameEngine/Resources/Textures/GeneralTexture.h"

namespace GameEngine
{
TreeRenderer::TreeRenderer(RendererContext& context)
    : context_(context)
    , leafsShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Tree)
    , trunkShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Entity)
{
}

void TreeRenderer::init()
{
    leafsShader_.Init();
    trunkShader_.Init();
}

void TreeRenderer::render()
{
    if (subscribes_.empty())
        return;

    trunkShader_.Start();

    for (auto& sub : subscribes_)
    {
        for (auto* treeRendererComponent_ : sub.treeRendererComponent_)
        {
            auto perUpdate = treeRendererComponent_->GetPerObjectUpdateId();
            if (perUpdate)
            {
                context_.graphicsApi_.BindShaderBuffer(*perUpdate);
            }

            if (treeRendererComponent_->GetInstancesSize() > 0)
            {
                RenderInstancedTree(*treeRendererComponent_);
            }
            else
            {
                RenderSingleTree(*treeRendererComponent_);
            }
        }
    }
}  // namespace GameEngine
void TreeRenderer::subscribe(GameObject& gameObject)
{
    auto component = gameObject.GetComponent<Components::TreeRendererComponent>();

    if (not component)
        return;

    auto iter = std::find_if(subscribes_.begin(), subscribes_.end(),
                             [&gameObject](const TreeSubscriber& sub) { return gameObject.GetId() == sub.gameObject_->GetId(); });

    if (iter == subscribes_.end())
    {
        subscribes_.push_back({&gameObject, {component}});
    }
    else
    {
        iter->treeRendererComponent_.push_back(component);
    }
}
void TreeRenderer::unSubscribe(GameObject& gameObject)
{
    auto sub = std::find_if(subscribes_.begin(), subscribes_.end(),
                            [&gameObject](const TreeSubscriber& sub) { return gameObject.GetId() == sub.gameObject_->GetId(); });

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
    leafsShader_.Reload();
    trunkShader_.Reload();
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
void TreeRenderer::BindMaterialTexture(uint32 location, GeneralTexture* texture, bool enabled) const
{
    if (enabled and texture and texture->GetGraphicsObjectId())
    {
        context_.graphicsApi_.ActiveTexture(location, *texture->GetGraphicsObjectId());
    }
}
void TreeRenderer::RenderSingleTree(const Components::TreeRendererComponent& treeRendererComponent)
{
    if (auto model = treeRendererComponent.GetModel().Get(LevelOfDetail::L1))
    {
        RenderModel(*model);
    }

    RenderLeafs(treeRendererComponent);
}
void TreeRenderer::RenderInstancedTree(const Components::TreeRendererComponent& treeRendererComponent)
{
    auto perInstance = treeRendererComponent.GetPerInstancesBufferId();
    if (perInstance)
    {
        context_.graphicsApi_.BindShaderBuffer(*perInstance);
    }

    if (auto model = treeRendererComponent.GetModel().Get(LevelOfDetail::L1))
    {
        RenderModel(*model, treeRendererComponent.GetInstancesSize());
    }
}

void TreeRenderer::RenderModel(const Model& model) const
{
    trunkShader_.Start();
    for (const auto& mesh : model.GetMeshes())
    {
        if (mesh.GetGraphicsObjectId())
        {
            const auto& buffer = mesh.GetMaterialShaderBufferId();
            context_.graphicsApi_.BindShaderBuffer(*buffer);
            RenderMesh(mesh);
        }
    }
    trunkShader_.Stop();
}
void TreeRenderer::RenderModel(const Model& model, uint32 count) const
{
    for (const auto& mesh : model.GetMeshes())
    {
        if (mesh.GetGraphicsObjectId())
        {
            const auto& buffer = mesh.GetMaterialShaderBufferId();
            context_.graphicsApi_.BindShaderBuffer(*buffer);
            RenderMesh(mesh, count);
        }
    }
}
void TreeRenderer::RenderMesh(const Mesh& mesh) const
{
    BindMaterial(mesh.GetMaterial());
    context_.graphicsApi_.RenderMesh(*mesh.GetGraphicsObjectId());
}
void TreeRenderer::RenderMesh(const Mesh& mesh, uint32 count) const
{
    BindMaterial(mesh.GetMaterial());
    context_.graphicsApi_.RenderMeshInstanced(*mesh.GetGraphicsObjectId(), count);
}
void TreeRenderer::RenderLeafs(const Components::TreeRendererComponent&) const
{
    return;

    leafsShader_.Start();
    context_.graphicsApi_.DisableCulling();
    // RenderModel(*treeRendererComponent_->GetTopModelWrapper().Get(LevelOfDetail::L1), sub);
    context_.graphicsApi_.EnableCulling();
    leafsShader_.Stop();
}
}  // namespace GameEngine
