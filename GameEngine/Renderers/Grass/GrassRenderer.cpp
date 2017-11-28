#include "GrassRenderer.h"

#include "../../Engine/Configuration.h"
#include "../../Engine/Projection.h"
#include "../../Objects/RenderAble/Flora/Grass/Grass.h"
#include "../../Scene/Scene.hpp"
#include "../Framebuffer/FrameBuffer.h"

#include "Logger/Log.h"
#include "OpenGL/OpenGLUtils.h"

CGrassRenderer::CGrassRenderer(CProjection* projection_matrix, CFrameBuffer* framebuffer) : CRenderer(framebuffer), projection(projection_matrix)
{
}

void CGrassRenderer::Init()
{
    shader.Init();
    shader.Start();
    shader.LoadProjectionMatrix(projection->GetProjectionMatrix());
    shader.Stop();

    viewDistance = EngineConf.floraViewDistance;
    Log("Grass renderer initialized.");
}

void CGrassRenderer::PrepareFrame(CScene* scene)
{
}

void CGrassRenderer::Render(CScene* scene)
{
    if (target == nullptr)
        return;

    PrepareRender(scene);
    RenderSubscribes();
    EndRender();
}

void CGrassRenderer::EndFrame(CScene* scene)
{
}

void CGrassRenderer::Subscribe(CGameObject* gameObject)
{
    auto grass = dynamic_cast<SGrass*>(gameObject);
    if (grass != nullptr)
        subscribes.push_back(grass);
}

void CGrassRenderer::PrepareRender(CScene* scene)
{
    target->BindToDraw();
    PrepareShader(scene);
    Utils::DisableCulling();
    glActiveTexture(GL_TEXTURE0);
}

void CGrassRenderer::EndRender() const
{
    Utils::EnableCulling();
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
    glBindTexture(GL_TEXTURE_2D, mesh.GetMaterial().diffuseTexture->GetId());
    glBindVertexArray(mesh.GetVao());
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_POINTS, 0, mesh.GetVertexCount());
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
}

void CGrassRenderer::PrepareShader(CScene* scene)
{
    shader.Start();
    shader.LoadGlobalTime(scene->GetGlobalTime());
    shader.LoadShadowValues(0.f, 0.f, 512.f);
    shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix());
    shader.LoadViewDistance(viewDistance);
}
