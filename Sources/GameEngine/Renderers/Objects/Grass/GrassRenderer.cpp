#include "GrassRenderer.h"
#include "GameEngine/Api/ShadersTypes.h"
#include "GameEngine/Components/Renderer/Grass/GrassComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Shaders/IShaderFactory.h"
#include "GameEngine/Shaders/IShaderProgram.h"
#include "Logger/Log.h"
#include "Shaders/GrassShaderUniforms.h"

namespace GameEngine
{
GrassRenderer::GrassRenderer(RendererContext& context)
    : context_(context)
{
    shader_ = context.shaderFactory_.create(Shaders::Grass);
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, GrassRenderer::Render);
}

GrassRenderer::~GrassRenderer()
{
}

void GrassRenderer::Init()
{
    InitShader();

    viewDistance = EngineConf.renderer.flora.viewDistance;
    Log("Grass renderer initialized.");
}

void GrassRenderer::Render(Scene* scene)
{
    if (subscribes_.empty())
        return;
    PrepareRender(scene);
    RenderSubscribes();
    EndRender();
}

void GrassRenderer::Subscribe(GameObject* gameObject)
{
    auto grass = gameObject->GetComponent<Components::GrassRendererComponent>();

    if (grass != nullptr)
    {
        subscribes_.push_back({gameObject->GetId(), grass});
    }
}

void GrassRenderer::ReloadShaders()
{
    shader_->Reload();
    InitShader();
}

void GrassRenderer::InitShader()
{
    shader_->Init();
    shader_->Start();
    shader_->Load(GrassShaderUniforms::ProjectionMatrix, context_.projection_.GetProjectionMatrix());
    shader_->Load(GrassShaderUniforms::ShadowVariables, vec3(0.f, 0.f, 512.f));
    shader_->Load(GrassShaderUniforms::ViewDistance, viewDistance);
    shader_->Stop();
}

void GrassRenderer::PrepareRender(Scene* scene)
{
    PrepareShader(scene);
    context_.graphicsApi_.DisableCulling();
}

void GrassRenderer::EndRender() const
{
    context_.graphicsApi_.EnableCulling();
}

void GrassRenderer::RenderSubscribes()
{
    for (const auto& s : subscribes_)
    {
        auto model = s.second->GetModel().Get();

        if (model == nullptr)
            continue;

        RenderModel(model);
    }
}

void GrassRenderer::RenderModel(Model* model)
{
    for (const auto& mesh : model->GetMeshes())
    {
        if (mesh.GetMaterial().diffuseTexture == nullptr)
            continue;

        RenderMesh(mesh);
    }
}

void GrassRenderer::RenderMesh(const Mesh& mesh)
{
    context_.graphicsApi_.ActiveTexture(0, mesh.GetMaterial().diffuseTexture->GetId());
    context_.graphicsApi_.RenderPoints(mesh.GetObjectId());
}

void GrassRenderer::PrepareShader(Scene* scene)
{
    shader_->Start();
    shader_->Load(GrassShaderUniforms::GlobalTime, scene->GetGlobalTime());
    shader_->Load(GrassShaderUniforms::ViewMatrix, scene->GetCamera()->GetViewMatrix());
}
}  // namespace GameEngine
