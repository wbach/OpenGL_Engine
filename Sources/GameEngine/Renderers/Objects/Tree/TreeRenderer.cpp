#include "TreeRenderer.h"
#include <algorithm>
#include "Common/Transform.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/PerInstances.h"
#include "GameEngine/Resources/ShaderBuffers/PerMeshObject.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"

namespace GameEngine
{
TreeRenderer::TreeRenderer(RendererContext& context)
    : context_(context)
    , shader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Tree)
{
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, TreeRenderer::Render);
}

void TreeRenderer::Init()
{
    shader_.Init();
}

void TreeRenderer::Render(const Scene& scene, const Time& threadTime)
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
void TreeRenderer::Subscribe(GameObject* gameObject)
{
    if (gameObject == nullptr)
        return;

    auto component = gameObject->GetComponent<Components::TreeRendererComponent>();

    if (component == nullptr)
        return;
    subscribes_.push_back({gameObject, component});
}
void TreeRenderer::UnSubscribe(GameObject* gameObject)
{
    auto sub = std::find_if(subscribes_.begin(), subscribes_.end(), [&gameObject](const TreeSubscriber& sub) {
        return gameObject->GetId() == sub.gameObject_->GetId();
    });

    if (sub != subscribes_.end())
    {
        subscribes_.erase(sub);
    }
}
void TreeRenderer::UnSubscribeAll()
{
}
void TreeRenderer::ReloadShaders()
{
    shader_.Reload();
}

void TreeRenderer::RenderModel(const Model& model, const TreeSubscriber& sub) const
{
    for (const auto& mesh : model.GetMeshes())
    {
        const auto& buffers = mesh.GetBuffers();
        context_.graphicsApi_.BindShaderBuffer(*buffers.perMeshObjectBuffer_);
        RenderMesh(mesh, sub.treeRendererComponent_->GetInstancesSize());
    }
}
void TreeRenderer::RenderMesh(const Mesh& mesh, uint32 size) const
{
    BindMaterial(mesh.GetMaterial());
    context_.graphicsApi_.RenderMeshInstanced(mesh.GetGraphicsObjectId(), size);
}
void TreeRenderer::RenderTrees()
{
}
void TreeRenderer::BindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.DisableCulling();

    if (material.diffuseTexture != nullptr && material.diffuseTexture->IsInitialized())
    {
        context_.graphicsApi_.ActiveTexture(1, material.diffuseTexture->GetGraphicsObjectId());
    }

    if (material.ambientTexture != nullptr && material.ambientTexture->IsInitialized())
    {
        context_.graphicsApi_.ActiveTexture(2, material.ambientTexture->GetGraphicsObjectId());
    }

    if (material.normalTexture != nullptr && material.normalTexture->IsInitialized())
    {
        context_.graphicsApi_.ActiveTexture(3, material.normalTexture->GetGraphicsObjectId());
    }

    if (material.specularTexture != nullptr && material.specularTexture->IsInitialized())
        context_.graphicsApi_.ActiveTexture(4, material.specularTexture->GetGraphicsObjectId());
}
void TreeRenderer::UnBindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.EnableCulling();
}
}  // namespace GameEngine
