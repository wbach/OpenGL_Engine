#include "GrassRenderer.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/RenderAble/Flora/Grass/Grass.h"
#include "GameEngine/Renderers/Framebuffer/FrameBuffer.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Resources/Models/Model.h"
#include "GameEngine/Resources/Models/Mesh.h"
#include "Logger/Log.h"

namespace GameEngine
{
GrassRenderer::GrassRenderer(RendererContext& context)
    : context_(context)
    , shader(context.graphicsApi_)
{
    __RegisterRenderFunction__(RendererFunctionType::UPDATE, GrassRenderer::Render);
}

void GrassRenderer::Init()
{
    InitShader();

    viewDistance = EngineConf.renderer.flora.viewDistance;
    Log("Grass renderer initialized.");
}

void GrassRenderer::Render(Scene* scene)
{
    PrepareRender(scene);
    RenderSubscribes();
    EndRender();
}

void GrassRenderer::Subscribe(GameObject* gameObject)
{
    auto grass = dynamic_cast<Grass*>(gameObject);
    if (grass != nullptr)
        subscribes.push_back(grass);
}

void GrassRenderer::ReloadShaders()
{
    shader.Reload();
    InitShader();
}

void GrassRenderer::InitShader()
{
    shader.Init();
    shader.Start();
    shader.LoadProjectionMatrix(context_.projection_->GetProjectionMatrix());
    shader.LoadShadowValues(0.f, 0.f, 512.f);
    shader.LoadViewDistance(viewDistance);
    shader.Stop();
}

void GrassRenderer::PrepareRender(Scene* scene)
{
    context_.defferedFrameBuffer_->BindToDraw();
    PrepareShader(scene);
    context_.graphicsApi_->DisableCulling();
}

void GrassRenderer::EndRender() const
{
    context_.graphicsApi_->EnableCulling();
    shader.Stop();
}

void GrassRenderer::RenderSubscribes()
{
    for (const auto& s : subscribes)
    {
        if (s->model == nullptr)
            continue;

        RenderModel(s->model);
    }
}

void GrassRenderer::RenderModel(CModel* model)
{
    for (const auto& mesh : model->GetMeshes())
    {
        if (mesh.GetMaterial().diffuseTexture == nullptr)
            continue;

        RenderMesh(mesh);
    }
}

void GrassRenderer::RenderMesh(const CMesh& mesh)
{
    context_.graphicsApi_->ActiveTexture(0, mesh.GetMaterial().diffuseTexture->GetId());
    context_.graphicsApi_->RenderPoints(mesh.GetObjectId());
}

void GrassRenderer::PrepareShader(Scene* scene)
{
    shader.Start();
    shader.LoadGlobalTime(scene->GetGlobalTime());
    shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix());
}
}  // GameEngine
