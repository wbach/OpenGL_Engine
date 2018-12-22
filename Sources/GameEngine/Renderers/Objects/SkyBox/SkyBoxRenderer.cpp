#include "SkyBoxRenderer.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Api/ShadersTypes.h"
#include "Shaders/SkyBoxShaderUniforms.h"
#include "Logger/Log.h"

namespace GameEngine
{
SkyBoxRenderer::SkyBoxRenderer(RendererContext& context)
    : context_(context)
    , model(nullptr)
    , dayTexture(nullptr)
    , nightTexture(nullptr)
    , resourceManager(context.graphicsApi_)
{
    shader_ = context.shaderFactory_.create(Shaders::SkyBox);
    __RegisterRenderFunction__(RendererFunctionType::CONFIGURE, SkyBoxRenderer::Render);
}

void SkyBoxRenderer::Init()
{
    InitShader();
//    Log("Skybox renderer initialized.");
}

void SkyBoxRenderer::PrepareToRendering(Scene* scene)
{
    context_.graphicsApi_->DisableCulling();
    shader_->Start();
    PrepareShaderBeforeFrameRender(scene);
}

void SkyBoxRenderer::EndRendering()
{
    context_.graphicsApi_->EnableCulling();
    shader_->Stop();
}

void SkyBoxRenderer::InitShader()
{
    shader_->Init();
    shader_->Start();
    shader_->Load(SkyBoxShaderUniforms::ProjectionMatrix, context_.projection_->GetProjectionMatrix());
    shader_->Load(SkyBoxShaderUniforms::FogColour, vec3(.8f, .8f, .8f));
    shader_->Load(SkyBoxShaderUniforms::BlendFactor, 1.f);
    shader_->Stop();
}

void SkyBoxRenderer::Render(Scene* scene)
{
    InitMembers(scene);

    if (!CheckModelIsReadyToRender())
        return;

    PrepareToRendering(scene);

    context_.defferedFrameBuffer_->BindToDraw();

    RenderSkyBoxModel();
    EndRendering();
}

bool SkyBoxRenderer::CheckModelIsReadyToRender()
{
    return model != nullptr && model->isLoadedToGpu();
}

void SkyBoxRenderer::PrepareShaderBeforeFrameRender(Scene* scene)
{
    mat4 viewMatrix = scene->GetCamera()->GetViewMatrix();
    // TO DO - delta time
    viewMatrix *= glm::scale(vec3(100.f));
   // viewMatrix *= glm::rotate((float)rotation_, .0f, 1.f, .0f);
    shader_->Load(SkyBoxShaderUniforms::ViewMatrix, viewMatrix);
    shader_->Load(SkyBoxShaderUniforms::BlendFactor, scene->GetDayNightCycle().GetDayNightBlendFactor());
}

void SkyBoxRenderer::RenderSkyBoxModel()
{
    const auto& meshes = model->GetMeshes();
    for (const auto& mesh : meshes)
        RenderSkyBoxMesh(mesh);
}

void SkyBoxRenderer::Subscribe(GameObject* gameObject)
{
}

void SkyBoxRenderer::ReloadShaders()
{
    shader_->Stop();
    shader_->Reload();
    InitShader();
}

void SkyBoxRenderer::InitMembers(Scene* scene)
{
    LoadModel(resourceManager);
    CreateDayTextures(resourceManager);
    CreateNightTextures(resourceManager);
}

void SkyBoxRenderer::LoadModel(ResourceManager& resource_manager)
{
    if (model != nullptr)
        return;

    model = resource_manager.LoadModel("Meshes/SkyBox/cube.obj");
    model->GpuLoadingPass();
}

void SkyBoxRenderer::CreateDayTextures(ResourceManager& resource_manager)
{
    if (dayTexture != nullptr)
        return;

    std::vector<std::string> dayTextures{"Skybox/TropicalSunnyDay/right.png", "Skybox/TropicalSunnyDay/left.png",
                                         "Skybox/TropicalSunnyDay/top.png",   "Skybox/TropicalSunnyDay/bottom.png",
                                         "Skybox/TropicalSunnyDay/back.png",  "Skybox/TropicalSunnyDay/front.png"};

    dayTexture = resource_manager.GetTextureLaoder().LoadCubeMap(dayTextures, false);
    dayTexture->GpuLoadingPass();
}

void SkyBoxRenderer::CreateNightTextures(ResourceManager& resource_manager)
{
    if (nightTexture != nullptr)
        return;

    std::vector<std::string> nightTextures{"Skybox/Night/right.png", "Skybox/Night/left.png",
                                           "Skybox/Night/top.png",   "Skybox/Night/bottom.png",
                                           "Skybox/Night/back.png",  "Skybox/Night/front.png"};

    nightTexture = resource_manager.GetTextureLaoder().LoadCubeMap(nightTextures, false);
    nightTexture->GpuLoadingPass();
}

void SkyBoxRenderer::BindTextures() const
{
    BindCubeMapTexture(dayTexture, 0);
    BindCubeMapTexture(nightTexture, 1);
}

void SkyBoxRenderer::BindCubeMapTexture(Texture* texture, int id) const
{
    if (texture == nullptr)
        return;

    context_.graphicsApi_->ActiveTexture(id, texture->GetId());
}

void SkyBoxRenderer::RenderSkyBoxMesh(const Mesh &mesh) const
{
    if (!mesh.IsInit())
        return;

    BindTextures();
    context_.graphicsApi_->RenderMesh(mesh.GetObjectId());
}
}  // GameEngine
