#include "SkyBoxRenderer.h"
#include "GameEngine/Components/Renderer/SkyBox/SkyBoxComponent.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/ShaderBuffers/PerObjectUpdate.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GraphicsApi/ShadersTypes.h"
#include "Logger/Log.h"
#include "Shaders/SkyBoxShaderUniforms.h"

namespace GameEngine
{
SkyBoxRenderer::SkyBoxRenderer(RendererContext& context)
    : context_(context)
    , rotationSpeed_(1.f)
    , rotation_(0.f)
    , scale_(150.f)
{
    shader_ = context.shaderFactory_.create(GraphicsApi::Shaders::SkyBox);
    __RegisterRenderFunction__(RendererFunctionType::CONFIGURE, SkyBoxRenderer::Render);
}

void SkyBoxRenderer::Init()
{
    InitShader();
    if (not perObjectUpdateId_)
    {
        perObjectUpdateId_ =
            context_.graphicsApi_.CreateShaderBuffer(PER_OBJECT_UPDATE_BIND_LOCATION, sizeof(PerObjectUpdate));
    }
    if (not perMeshObjectId_)
    {
        perMeshObjectId_ = context_.graphicsApi_.CreateShaderBuffer(PER_MESH_OBJECT_BIND_LOCATION,
                                                                    sizeof(SkyBoxRenderer::PerMeshObject));

        perMeshObject_.blendFactor_ = 1.f;
        perMeshObject_.fogColor_    = vec4(.8f, .8f, .8f, 1.f);
    }
}

void SkyBoxRenderer::PrepareToRendering(const Scene& scene)
{
    shader_->Start();
    PrepareShaderBeforeFrameRender(scene);
}

void SkyBoxRenderer::InitShader()
{
    shader_->Init();
}

void SkyBoxRenderer::UpdateBuffer(const Scene& scene, const Time& threadTime)
{
    rotation_.y += threadTime.deltaTime * rotationSpeed_;

    perObjectUpdateBuffer_.TransformationMatrix =
        Utils::CreateTransformationMatrix(scene.GetCamera().GetPosition(), rotation_, scale_);

    context_.graphicsApi_.UpdateShaderBuffer(*perObjectUpdateId_, &perObjectUpdateBuffer_);
    context_.graphicsApi_.BindShaderBuffer(*perObjectUpdateId_);
}

void SkyBoxRenderer::Render(const Scene& scene, const Time& threadTime)
{
    if (subscribes_.empty())
        return;

    UpdateBuffer(scene, threadTime);
    PrepareToRendering(scene);

    for (const auto& subscriber : subscribes_)
    {
        RenderSkyBoxModel(subscriber.second);
    }
}

void SkyBoxRenderer::PrepareShaderBeforeFrameRender(const Scene& scene)
{
    perMeshObject_.blendFactor_ = scene.GetDayNightCycle().GetDayNightBlendFactor();
    context_.graphicsApi_.UpdateShaderBuffer(*perMeshObjectId_, &perMeshObject_);
    context_.graphicsApi_.BindShaderBuffer(*perMeshObjectId_);
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
    BindCubeMapTexture(*sub.dayTexture_, 0);
    BindCubeMapTexture(*sub.nightTexture_, 1);
}

void SkyBoxRenderer::BindCubeMapTexture(const Texture& texture, int id) const
{
    context_.graphicsApi_.ActiveTexture(id, texture.GetGraphicsObjectId());
}

void SkyBoxRenderer::RenderSkyBoxMesh(const Mesh& mesh) const
{
    if (!mesh.IsInit())
        return;

    context_.graphicsApi_.RenderMesh(mesh.GetGraphicsObjectId());
}
}  // namespace GameEngine
