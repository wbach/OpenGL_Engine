#include "GrassRenderer.h"
#include "GameEngine/Scene/Scene.hpp"
#include "../Framebuffer/FrameBuffer.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Objects/RenderAble/Flora/Grass/Grass.h"
#include "Logger/Log.h"

CGrassRenderer::CGrassRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection_matrix, CFrameBuffer* framebuffer)
	: CRenderer(framebuffer)
	, graphicsApi_(graphicsApi)
	, shader(graphicsApi)
	, projection(projection_matrix)
{
}

void CGrassRenderer::Init()
{
	InitShader();

    viewDistance = EngineConf.floraViewDistance;
    Log("Grass renderer initialized.");
}

void CGrassRenderer::PrepareFrame(GameEngine::Scene* scene)
{
}

void CGrassRenderer::Render(GameEngine::Scene* scene)
{
    if (target == nullptr)
        return;

    PrepareRender(scene);
    RenderSubscribes();
    EndRender();
}

void CGrassRenderer::EndFrame(GameEngine::Scene* scene)
{
}

void CGrassRenderer::Subscribe(CGameObject* gameObject)
{
    auto grass = dynamic_cast<SGrass*>(gameObject);
    if (grass != nullptr)
        subscribes.push_back(grass);
}

void CGrassRenderer::ReloadShaders()
{
	shader.Reload();
	InitShader();
}

void CGrassRenderer::InitShader()
{
	shader.Init();
	shader.Start();
	shader.LoadProjectionMatrix(projection->GetProjectionMatrix());
	shader.LoadShadowValues(0.f, 0.f, 512.f);
	shader.LoadViewDistance(viewDistance);
	shader.Stop();
}

void CGrassRenderer::PrepareRender(GameEngine::Scene* scene)
{
    target->BindToDraw();
    PrepareShader(scene);
	graphicsApi_->DisableCulling();
}

void CGrassRenderer::EndRender() const
{
	graphicsApi_->EnableCulling();
    shader.Stop();
}

void CGrassRenderer::RenderSubscribes()
{
    for (const auto& s : subscribes)
    {
        if (s->model == nullptr)
            continue;

        RenderModel(s->model);
    }
}

void CGrassRenderer::RenderModel(CModel* model)
{
    for (const auto& mesh : model->GetMeshes())
    {
        if (mesh.GetMaterial().diffuseTexture == nullptr)
            continue;

        RenderMesh(mesh);
    }
}

void CGrassRenderer::RenderMesh(const CMesh& mesh)
{
	graphicsApi_->ActiveTexture(0, mesh.GetMaterial().diffuseTexture->GetId());
	graphicsApi_->RenderPoints(mesh.GetObjectId());
}

void CGrassRenderer::PrepareShader(GameEngine::Scene* scene)
{
    shader.Start();
    shader.LoadGlobalTime(scene->GetGlobalTime());
    shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix());
}
