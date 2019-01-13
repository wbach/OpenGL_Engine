#include "SkyBoxRenderer.h"
#include "GameEngine/Api/ShadersTypes.h"
#include "GameEngine/Components/Renderer/SkyBox/SkyBoxComponent.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "Logger/Log.h"
#include "Shaders/SkyBoxShaderUniforms.h"

namespace GameEngine
{
SkyBoxRenderer::SkyBoxRenderer(RendererContext& context)
    : context_(context)
    , rotation_(0.f)
{
    shader_ = context.shaderFactory_.create(Shaders::SkyBox);
    __RegisterRenderFunction__(RendererFunctionType::CONFIGURE, SkyBoxRenderer::Render);
}

void SkyBoxRenderer::Init()
{
    InitShader();
}

void SkyBoxRenderer::PrepareToRendering(Scene* scene)
{
    context_.graphicsApi_.DisableCulling();
    shader_->Start();
    PrepareShaderBeforeFrameRender(scene);
}

void SkyBoxRenderer::EndRendering()
{
    context_.graphicsApi_.EnableCulling();
}

void SkyBoxRenderer::InitShader()
{
    shader_->Init();
    shader_->Start();
    shader_->Load(SkyBoxShaderUniforms::ProjectionMatrix, context_.projection_.GetProjectionMatrix());
    shader_->Load(SkyBoxShaderUniforms::FogColour, vec3(.8f, .8f, .8f));
    shader_->Load(SkyBoxShaderUniforms::BlendFactor, 1.f);
    shader_->Stop();
}

mat4 SkyBoxRenderer::ModifiedViewMatrix(const mat4& viewMatrix) const
{
    mat4 newViewMatrix = viewMatrix;

    newViewMatrix[3][0] = 0;
    newViewMatrix[3][1] = 0;
    newViewMatrix[3][2] = 0;

    newViewMatrix *= glm::scale(vec3(150.f));
    newViewMatrix *= glm::rotate(rotation_, .0f, 1.f, .0f);

    return newViewMatrix;
}

void SkyBoxRenderer::Render(Scene* scene)
{
    rotation_ += 0.01f;
    PrepareToRendering(scene);

    for (const auto& subscriber : subscribes_)
    {
        RenderSkyBoxModel(subscriber.second);
    }
    EndRendering();
}

void SkyBoxRenderer::PrepareShaderBeforeFrameRender(Scene* scene)
{
    shader_->Load(SkyBoxShaderUniforms::ViewMatrix, ModifiedViewMatrix(scene->GetCamera()->GetViewMatrix()));
    shader_->Load(SkyBoxShaderUniforms::BlendFactor, scene->GetDayNightCycle().GetDayNightBlendFactor());
}

void SkyBoxRenderer::RenderSkyBoxModel(const SkyBoxSubscriber& sub)
{
    const auto& meshes = sub.model_->GetMeshes();

    BindTextures(sub);

    for (const auto& mesh : meshes)
    {
        RenderSkyBoxMesh(mesh);
    }
}

void SkyBoxRenderer::Subscribe(GameObject* gameObject)
{
    auto component = gameObject->GetComponent<Components::SkyBoxComponent>();

    if (component == nullptr)
        return;

    subscribes_.insert(
        {gameObject->GetId(), {component->GetModel(), component->GetDayTexture(), component->GetNightTexture()}});
}

void SkyBoxRenderer::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    InitShader();
}

void SkyBoxRenderer::BindTextures(const SkyBoxSubscriber& sub) const
{
    BindCubeMapTexture(sub.dayTexture_, 0);
    BindCubeMapTexture(sub.nightTexture_, 1);
}

void SkyBoxRenderer::BindCubeMapTexture(Texture* texture, int id) const
{
    if (texture == nullptr)
        return;

    context_.graphicsApi_.ActiveTexture(id, texture->GetId());
}

void SkyBoxRenderer::RenderSkyBoxMesh(const Mesh& mesh) const
{
    if (!mesh.IsInit())
        return;

    context_.graphicsApi_.RenderMesh(mesh.GetObjectId());
}
}  // namespace GameEngine
