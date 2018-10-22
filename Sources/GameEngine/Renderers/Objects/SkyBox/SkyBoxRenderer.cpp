#include "SkyBoxRenderer.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Textures/Texture.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"

namespace GameEngine
{
SkyBoxRenderer::SkyBoxRenderer(RendererContext& context)
    : context_(context)
    , shader(context.graphicsApi_)
    , model(nullptr)
    , dayTexture(nullptr)
    , nightTexture(nullptr)
    , resourceManager(context.graphicsApi_)
{
    __RegisterRenderFunction__(RendererFunctionType::CONFIGURE, SkyBoxRenderer::Render);
}

void SkyBoxRenderer::Init()
{
    InitShader();
    Log("Skybox renderer initialized.");
}

void SkyBoxRenderer::PrepareToRendering(Scene* scene)
{
    context_.graphicsApi_->DisableCulling();
    shader.Start();
    PrepareShaderBeforeFrameRender(scene);
}

void SkyBoxRenderer::EndRendering()
{
    context_.graphicsApi_->EnableCulling();
    shader.Stop();
}

void SkyBoxRenderer::InitShader()
{
    shader.Init();
    shader.Start();
    shader.LoadProjectionMatrix(context_.projection_->GetProjectionMatrix());
    shader.LoadFogColour(.8f, .8f, .8f);
    shader.LoadBlendFactor(1.f);
    shader.Stop();
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
    return model != nullptr && model->isInOpenGL();
}

void SkyBoxRenderer::PrepareShaderBeforeFrameRender(Scene* scene)
{
    // TO DO - delta time
    shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix(), 0.1f, 1500.f);
    shader.LoadBlendFactor(scene->GetDayNightCycle().GetDayNightBlendFactor());
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
    shader.Stop();
    shader.Reload();
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
    model->OpenGLLoadingPass();
}

void SkyBoxRenderer::CreateDayTextures(ResourceManager& resource_manager)
{
    if (dayTexture != nullptr)
        return;

    std::vector<std::string> dayTextures{"Skybox/TropicalSunnyDay/right.png", "Skybox/TropicalSunnyDay/left.png",
                                         "Skybox/TropicalSunnyDay/top.png",   "Skybox/TropicalSunnyDay/bottom.png",
                                         "Skybox/TropicalSunnyDay/back.png",  "Skybox/TropicalSunnyDay/front.png"};

    dayTexture = resource_manager.GetTextureLaoder().LoadCubeMap(dayTextures, false);
    dayTexture->OpenGLLoadingPass();
}

void SkyBoxRenderer::CreateNightTextures(ResourceManager& resource_manager)
{
    if (nightTexture != nullptr)
        return;

    std::vector<std::string> nightTextures{"Skybox/Night/right.png", "Skybox/Night/left.png",
                                           "Skybox/Night/top.png",   "Skybox/Night/bottom.png",
                                           "Skybox/Night/back.png",  "Skybox/Night/front.png"};

    nightTexture = resource_manager.GetTextureLaoder().LoadCubeMap(nightTextures, false);
    nightTexture->OpenGLLoadingPass();
}

void SkyBoxRenderer::BindTextures() const
{
    BindCubeMapTexture(dayTexture, 0);
    BindCubeMapTexture(nightTexture, 1);
}

void SkyBoxRenderer::BindCubeMapTexture(CTexture* texture, int id) const
{
    if (texture == nullptr)
        return;

    context_.graphicsApi_->ActiveTexture(id, texture->GetId());
}

void SkyBoxRenderer::RenderSkyBoxMesh(const CMesh& mesh) const
{
    if (!mesh.IsInit())
        return;

    BindTextures();
    context_.graphicsApi_->RenderMesh(mesh.GetObjectId());
}
}  // GameEngine
