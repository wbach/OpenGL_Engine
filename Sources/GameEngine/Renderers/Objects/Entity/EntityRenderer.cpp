#include "EntityRenderer.h"
#include "GameEngine/Components/Renderer/RendererComponent.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Engine/EngineMeasurement.h"
#include "GameEngine/Objects/RenderAble/Entity/Entity.h"
#include "GameEngine/Renderers/Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
#include "Shaders/EntityShaderUniforms.h"
#include "Utils/Time/Timer.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Api/ShadersTypes.h"
#include "EntityRendererDef.h"

namespace GameEngine
{
EntityRenderer::EntityRenderer(RendererContext& context)
    : context_(context)
{
    shader_ = context.shaderFactory_.create(Shaders::Entity);
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, EntityRenderer::Render);
}

void EntityRenderer::Init()
{
    Log("Start initialize entity renderer...");
    InitShader();
    Log("EntityRenderer initialized.");
}

void EntityRenderer::InitShader()
{
    shader_->Init();
    shader_->Start();
    shader_->Load(EntityShaderUniforms::ViewDistance, EntityRendererDef::DEFAULT_VIEW_DISTANCE);
    shader_->Load(EntityShaderUniforms::ShadowVariables, EntityRendererDef::DEFAULT_SHADOW_VARIABLES);
    shader_->Load(EntityShaderUniforms::ClipPlane, EntityRendererDef::DEFAULT_CLIP_PLANE);
    shader_->Load(EntityShaderUniforms::IsUseFakeLighting, false);
    shader_->Load(EntityShaderUniforms::ProjectionMatrix, context_.projection_->GetProjectionMatrix());
    shader_->Stop();
}

void EntityRenderer::PrepareFrame(Scene* scene)
{
    shader_->Load(EntityShaderUniforms::ViewMatrix, scene->GetCamera()->GetViewMatrix());
}

void EntityRenderer::Render(Scene* scene)
{
    Utils::Timer timer;
    context_.defferedFrameBuffer_->BindToDraw();
    shader_->Start();
    PrepareFrame(scene);
    RenderEntities();
    shader_->Stop();
    context_.defferedFrameBuffer_->UnBind();
    MakeMeasurement("EntityRenderer", timer.GetTimeMiliseconds());
}

void EntityRenderer::Subscribe(GameObject* gameObject)
{
    auto rendererComponent = gameObject->GetComponent<Components::RendererComponent>();

    if (rendererComponent == nullptr)
        return;

    subscribes_[gameObject->GetId()] = {rendererComponent->textureIndex, gameObject,
                                        &rendererComponent->GetModelWrapper()};
}

void EntityRenderer::UnSubscribe(GameObject* gameObject)
{
    subscribes_.erase(gameObject->GetId());
}

void EntityRenderer::UnSubscribeAll()
{
    subscribes_.clear();
}

void EntityRenderer::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    InitShader();
}

void EntityRenderer::RenderModel(Model* model, const mat4& transform_matrix) const
{
    const auto& meshes = model->GetMeshes();
    for (const auto& mesh : meshes)
    {
        if (mesh.UseArmature())
        {
            shader_->Load(EntityShaderUniforms::BonesTransforms, model->GetBoneTransforms());
        }
        shader_->Load(EntityShaderUniforms::UseBoneTransform, mesh.UseArmature());
        RenderMesh(mesh, transform_matrix);
    }
}

void EntityRenderer::RenderMesh(const Mesh& mesh, const mat4& transform_matrix) const
{
    BindMaterial(mesh.GetMaterial());
    auto transform_matrix_ = transform_matrix * mesh.GetMeshTransform();
    shader_->Load(EntityShaderUniforms::TransformationMatrix, transform_matrix_);
    context_.graphicsApi_->RenderMesh(mesh.GetObjectId());
    UnBindMaterial(mesh.GetMaterial());
}

void EntityRenderer::RenderEntities()
{
    for (auto& sub : subscribes_)
    {
        if (sub.second.gameObject == nullptr || sub.second.model == nullptr)
            Log("[Error] Null subsciber in EnityRenderer.");

        auto model = sub.second.model->Get(LevelOfDetail::L1);

        if (model == nullptr)
            continue;

        uint32 x = 0;
        /* for (auto& t : model->GetBoneTransforms())
             shader.LoadBoneTransform(*t, x++);
             */


        currentTextureIndex_ = sub.second.textureIndex;

        RenderModel(model, sub.second.gameObject->worldTransform.GetMatrix());
    }
}

void EntityRenderer::BindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_->DisableCulling();

    shader_->Load(EntityShaderUniforms::ModelMaterialAmbient, material.ambient);
    shader_->Load(EntityShaderUniforms::ModelMaterialDiffuse, material.diffuse);
    shader_->Load(EntityShaderUniforms::ModelMaterialSpecular, material.specular);

    if (material.diffuseTexture != nullptr && material.diffuseTexture->IsInitialized() &&
        EngineConf.renderer.textures.useDiffuse)
    {
        shader_->Load(EntityShaderUniforms::UseTexture, true);
        shader_->Load(EntityShaderUniforms::NumberOfRows, material.diffuseTexture->numberOfRows);
        shader_->Load(EntityShaderUniforms::TextureOffset,
                      material.diffuseTexture->GetTextureOffset(currentTextureIndex_));
        context_.graphicsApi_->ActiveTexture(0, material.diffuseTexture->GetId());
    }
    else
    {
        shader_->Load(EntityShaderUniforms::UseTexture, false);
    }

    if (material.ambientTexture != nullptr && material.ambientTexture->IsInitialized() &&
        EngineConf.renderer.textures.useAmbient)
    {
        context_.graphicsApi_->ActiveTexture(1, material.ambientTexture->GetId());
    }

    if (material.normalTexture != nullptr && material.normalTexture->IsInitialized() &&
        EngineConf.renderer.textures.useNormal)
    {
        context_.graphicsApi_->ActiveTexture(2, material.normalTexture->GetId());
        shader_->Load(EntityShaderUniforms::IsUseNormalMap, true);
    }
    else
    {
        shader_->Load(EntityShaderUniforms::IsUseNormalMap, false);
    }

    if (material.specularTexture != nullptr && material.specularTexture->IsInitialized() &&
        EngineConf.renderer.textures.useSpecular)
    {
        context_.graphicsApi_->ActiveTexture(3, material.specularTexture->GetId());
    }

}

void EntityRenderer::UnBindMaterial(const Material& material) const
{
    if (material.isTransparency)
        context_.graphicsApi_->EnableCulling();
}
}  // namespace GameEngine
