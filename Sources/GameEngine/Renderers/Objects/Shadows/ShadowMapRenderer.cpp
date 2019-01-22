#include "ShadowMapRenderer.hpp"
#include "GLM/GLMUtils.h"
#include "GraphicsApi/ShadersTypes.h"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "Logger/Log.h"
#include "Shaders/ShadowShaderUniforms.h"
#include "ShadowFrameBuffer.h"
#include "math.hpp"
namespace GameEngine
{
ShadowMapRenderer::ShadowMapRenderer(RendererContext& context)
    : context_(context)
    , shadowBox_(context.projection_)
    , shadowBox2_(context.projection_)
    , projectionViewMatrix_(1.f)
    , viewOffset_(Utils::CreateOffset())
{
    shader_ = context.shaderFactory_.create(GraphicsApi::Shaders::Shadows);
    __RegisterRenderFunction__(RendererFunctionType::PRERENDER, ShadowMapRenderer::Render);
}

void ShadowMapRenderer::Init()
{
    shader_->Init();
}

void ShadowMapRenderer::Render(Scene* scene)
{
    PrepareRender(scene);
    PrepareShader(scene->GetCamera());
    RenderSubscribes();
    context_.shadowsFrameBuffer_.UnbindFrameBuffer();
}

void ShadowMapRenderer::Subscribe(GameObject* gameObject)
{
    auto rendererComponent = gameObject->GetComponent<Components::RendererComponent>();

    if (rendererComponent == nullptr)
        return;

    subscribes_.insert({gameObject->GetId(),
                        {rendererComponent->GetTextureIndex(), gameObject, &rendererComponent->GetModelWrapper()}});
}

void ShadowMapRenderer::UnSubscribe(GameObject* gameObject)
{
    subscribes_.erase(gameObject->GetId());
}

void ShadowMapRenderer::UnSubscribeAll()
{
    subscribes_.clear();
}

void ShadowMapRenderer::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    shader_->Init();
}

void ShadowMapRenderer::PrepareRender(Scene* scene)
{
    context_.shadowsFrameBuffer_.BindFBO();
    context_.graphicsApi_.EnableDepthTest();
    context_.graphicsApi_.ClearBuffer(GraphicsApi::BufferType::DEPTH);
    shadowBox_.Update(scene->GetCamera());

    auto cameraPos = scene->GetCamera()->GetPosition();
    scene->GetCamera()->SetPosition(vec3(0, 0, 0));
    shadowBox2_.Update(scene->GetCamera());
    scene->GetCamera()->SetPosition(cameraPos);

    auto light_direction = scene->GetDirectionalLight().GetDirection();
    shadowBox_.CalculateMatrixes(light_direction);
    shadowBox2_.CalculateMatrixes(light_direction);

    context_.toShadowMapZeroMatrix_ = viewOffset_ * shadowBox2_.GetProjectionViewMatrix();
}

void ShadowMapRenderer::RenderSubscribes() const
{
    for (auto& sub : subscribes_)
        RenderSubscriber(sub.second);
}

void ShadowMapRenderer::RenderSubscriber(const ShadowMapSubscriber& sub) const
{
    auto model = sub.model->Get(LevelOfDetail::L1);

    if (model == nullptr)
        return;

    const auto& meshes = model->GetMeshes();

    for (const Mesh& mesh : meshes)
    {
        if (mesh.UseArmature())
        {
            shader_->Load(ShadowShaderUniforms::BonesTransforms, model->GetBoneTransforms());
        }
        shader_->Load(ShadowShaderUniforms::UseBoneTransform, mesh.UseArmature());

        RenderMesh(mesh, sub.gameObject->worldTransform.GetMatrix(), sub.textureIndex);
    }
}

void ShadowMapRenderer::RenderMesh(const Mesh& mesh, const mat4& transform_matrix, uint32 textureIndex) const
{
    if (!mesh.IsInit())
        return;

    auto transform_matrix_ = transform_matrix * mesh.GetMeshTransform();
    BindMaterial(mesh.GetMaterial(), textureIndex);

    shader_->Load(ShadowShaderUniforms::TransformationMatrix, transform_matrix_);

    context_.graphicsApi_.RenderMesh(mesh.GetObjectId());
}

void ShadowMapRenderer::BindMaterial(const Material& material, uint32 textureIndex) const
{
    if (material.diffuseTexture == nullptr)
        return;

    shader_->Load(ShadowShaderUniforms::NumberOfRows, static_cast<float>(material.diffuseTexture->numberOfRows));
    shader_->Load(ShadowShaderUniforms::TextureOffset, material.diffuseTexture->GetTextureOffset(textureIndex));
    context_.graphicsApi_.ActiveTexture(0, material.diffuseTexture->GetId());
}

void ShadowMapRenderer::PrepareShader(ICamera*) const
{
    shader_->Start();
    shader_->Load(ShadowShaderUniforms::ProjectionViewMatrix, shadowBox_.GetProjectionViewMatrix());
}
}  // namespace GameEngine
