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
#include "Utils/Time/Timer.h"

namespace GameEngine
{
EntityRenderer::EntityRenderer(RendererContext& context)
    : context_(context)
    , shader(context.graphicsApi_)
    , clipPlane(vec4(0, 1, 0, 100000))
{
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
    shader.Init();
    shader.Start();
    shader.LoadViewDistance(500.f);
    shader.LoadClipPlane(clipPlane);
    shader.LoadShadowValues(0.f, 10.f, 10.f);
    shader.LoadUseFakeLight(0.f);
    shader.LoadProjectionMatrix(context_.projection_->GetProjectionMatrix());
    shader.Stop();
}

void EntityRenderer::PrepareFrame(Scene* scene)
{
    shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix());
    shader.LoadUseBonesTransformation(0.f);
}

void EntityRenderer::Render(Scene* scene)
{
    Utils::Timer timer;
    context_.defferedFrameBuffer_->BindToDraw();
    shader.Start();
    PrepareFrame(scene);
    RenderEntities();
    shader.Stop();
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
    shader.Stop();
    shader.Reload();
    InitShader();
}

void EntityRenderer::RenderModel(Model *model, const mat4& transform_matrix) const
{
    const auto& meshes = model->GetMeshes();
    for (const auto& mesh : meshes)
        RenderMesh(mesh, transform_matrix);
}

void EntityRenderer::RenderMesh(const Mesh &mesh, const mat4& transform_matrix) const
{
    BindMaterial(mesh.GetMaterial());
    auto transform_matrix_ = transform_matrix * mesh.GetMeshTransform();
    shader.LoadTransformMatrix(transform_matrix_);
    shader.LoadUseBonesTransformation(static_cast<float>(mesh.UseArmature()));
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
        for (auto& t : model->GetBoneTransforms())
            shader.LoadBoneTransform(*t, x++);

        currentTextureIndex_ = sub.second.textureIndex;

        RenderModel(model, sub.second.gameObject->worldTransform.GetMatrix());
    }
}

void EntityRenderer::BindMaterial(const Material &material) const
{
    if (material.isTransparency)
        context_.graphicsApi_->DisableCulling();

    shader.LoadMeshMaterial(material);

    if (material.diffuseTexture != nullptr && material.diffuseTexture->IsInitialized() &&
        EngineConf.renderer.textures.useDiffuse)
    {
        shader.LoadUseTexture(1.f);
        context_.graphicsApi_->ActiveTexture(0, material.diffuseTexture->GetId());
        shader.LoadNumberOfRows(material.diffuseTexture->numberOfRows);
        shader.LoadTextureOffset(material.diffuseTexture->GetTextureOffset(currentTextureIndex_));
    }
    else
    {
        shader.LoadUseTexture(0.f);
        shader.LoadNumberOfRows(1);
        shader.LoadTextureOffset(vec2(0));
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
        shader.LoadUseNormalMap(1.f);
    }
    else
        shader.LoadUseNormalMap(0.f);

    if (material.specularTexture != nullptr && material.specularTexture->IsInitialized() &&
        EngineConf.renderer.textures.useSpecular)
        context_.graphicsApi_->ActiveTexture(3, material.specularTexture->GetId());
}

void EntityRenderer::UnBindMaterial(const Material &material) const
{
    if (material.isTransparency)
        context_.graphicsApi_->EnableCulling();
}
}  // GameEngine
