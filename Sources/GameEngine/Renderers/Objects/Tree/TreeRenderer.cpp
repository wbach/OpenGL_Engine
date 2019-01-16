#include "TreeRenderer.h"
#include "Common/Transform.h"
#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "Shaders/TreeShaderUniforms.h"

namespace GameEngine
{
TreeRenderer::TreeRenderer(RendererContext& context)
    : context_(context)
{
    shader_ = context.shaderFactory_.create(Shaders::Tree);
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, TreeRenderer::Render);
}
void TreeRenderer::Init()
{
    shader_->Init();
    shader_->Start();
    shader_->Load(TreeShaderUniforms::ProjectionMatrix, context_.projection_.GetProjectionMatrix());
    shader_->Stop();
}
void TreeRenderer::Render(Scene* scene)
{
    if (subscribes_.empty())
        return;

    for (auto iter = subscribersToInit_.begin(); iter != subscribersToInit_.end();)
    {
        PreparePositionMap(**iter);
        iter = subscribersToInit_.erase(iter);
    }

    shader_->Start();
    shader_->Load(TreeShaderUniforms::ViewMatrix, scene->GetCamera()->GetViewMatrix());
    shader_->Load(TreeShaderUniforms::CameraPosition, scene->GetCamera()->GetPosition());

    for (auto& sub : subscribes_)
    {
        // PreparePositionMap(sub.second);

        const auto& model  = sub.second.top->Get(L1);
        const auto& bmodel = sub.second.bottom->Get(L1);

        float factor =
            model->GetScaleFactor() > bmodel->GetScaleFactor() ? model->GetScaleFactor() : bmodel->GetScaleFactor();
        mat4 normalizeMatrix = glm::scale(vec3(1.f / factor)) * sub.second.transform;

        uint32 count = sub.second.positions->size();

        context_.graphicsApi_.ActiveTexture(0, sub.second.positionTexture);
        shader_->Load(TreeShaderUniforms::PositionMapSize, count);
        context_.graphicsApi_.DisableCulling();
        shader_->Load(TreeShaderUniforms::UseShading, 0.f);
        RenderModel(model, normalizeMatrix, count);
        context_.graphicsApi_.EnableCulling();
        shader_->Load(TreeShaderUniforms::UseShading, 1.f);
        RenderModel(bmodel, normalizeMatrix, count);
    }
}
void TreeRenderer::Subscribe(GameObject* gameObject)
{
    if (gameObject == nullptr)
        return;

    auto component = gameObject->GetComponent<Components::TreeRendererComponent>();

    if (component == nullptr)
        return;

    auto& sub     = subscribes_[gameObject->GetId()];
    sub.top       = &component->GetTopModelWrapper();
    sub.bottom    = &component->GetBottomModelWrapper();
    sub.positions = &component->GetPositions();
    sub.transform = gameObject->worldTransform.GetMatrix();
    subscribersToInit_.push_back(&sub);
}
void TreeRenderer::UnSubscribe(GameObject* gameObject)
{
}
void TreeRenderer::UnSubscribeAll()
{
}
void TreeRenderer::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    Init();
}
void TreeRenderer::PreparePositionMap(TreeSubscriber& sub)
{
    if (sub.textureInGpu)
        return;

    sub.positionTexture = context_.graphicsApi_.CreateTexture(TextureType::FLOAT_TEXTURE_3C, TextureFilter::NEAREST,
                                                               TextureMipmap::NONE, BufferAtachment::NONE,
                                                               vec2ui(sub.positions->size(), 1), &(*sub.positions)[0]);
    sub.textureInGpu = true;
}
void TreeRenderer::RenderModel(Model* model, const mat4& transorm, uint32 size) const
{
    for (const auto& mesh : model->GetMeshes())
        RenderMesh(mesh, transorm, size);
}
void TreeRenderer::RenderMesh(const Mesh& mesh, const mat4& transform, uint32 size) const
{
    shader_->Load(TreeShaderUniforms::NormalizationMatrix, transform);
    BindMaterial(mesh.GetMaterial());
    context_.graphicsApi_.RenderMeshInstanced(mesh.GetObjectId(), size);
}
void TreeRenderer::RenderTrees()
{
}
void TreeRenderer::BindMaterial(const Material &material) const
{
    if (material.isTransparency)
        context_.graphicsApi_.DisableCulling();

    shader_->Load(TreeShaderUniforms::ModelMaterial_Ambient, material.ambient);
    shader_->Load(TreeShaderUniforms::ModelMaterial_Diffuse, material.diffuse);
    shader_->Load(TreeShaderUniforms::ModelMaterial_Specular, material.specular);
    shader_->Load(TreeShaderUniforms::ModelMaterial_ShineDumper, material.shineDamper);

    if (material.diffuseTexture != nullptr && material.diffuseTexture->IsInitialized())
    {
        context_.graphicsApi_.ActiveTexture(1, material.diffuseTexture->GetId());
    }

    if (material.ambientTexture != nullptr && material.ambientTexture->IsInitialized())
    {
        context_.graphicsApi_.ActiveTexture(2, material.ambientTexture->GetId());
    }

    if (material.normalTexture != nullptr && material.normalTexture->IsInitialized())
    {
        context_.graphicsApi_.ActiveTexture(3, material.normalTexture->GetId());
    }

    if (material.specularTexture != nullptr && material.specularTexture->IsInitialized())
        context_.graphicsApi_.ActiveTexture(4, material.specularTexture->GetId());
}
void TreeRenderer::UnBindMaterial(const Material &material) const
{
}
}  // GameEngine
