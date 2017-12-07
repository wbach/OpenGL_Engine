#include "ShadowMapRenderer.hpp"
#include "ShadowFrameBuffer.h"
#include "Engine/Projection.h"
#include "../../Objects/RenderAble/Entity/Entity.h"
#include "Camera/Camera.h"
#include "GLM/GLMUtils.h"
#include "OpenGL/OpenGLUtils.h"
#include "math.hpp"

#include "Engine/Configuration.h"

CShadowMapRenderer::CShadowMapRenderer(CProjection* projection, CShadowFrameBuffer* framebuffer)
: shadowBox(projection)
, projection(projection)
, shadowFrameBuffer(framebuffer)
, projectionViewMatrix(1.f)
{
    EngineConf.texturesIds.push_back(shadowFrameBuffer->GetShadowMap());
}

void CShadowMapRenderer::Init()
{
    shader.Init();
}

void CShadowMapRenderer::PrepareFrame(CScene* scene)
{

}

void CShadowMapRenderer::Render(CScene *scene)
{
    PrepareRender(scene);
    PrepareShader(scene->GetCamera());
    RenderSubscribes();
    shader.Stop();
    shadowFrameBuffer->UnbindFrameBuffer();
}

void CShadowMapRenderer::EndFrame(CScene* scene)
{

}

void CShadowMapRenderer::Subscribe(CGameObject* gameObject)
{
    auto entity = dynamic_cast<CEntity*>(gameObject);

    if (entity == nullptr)
        return;

    subscribes.push_back((entity));
}

void CShadowMapRenderer::PrepareRender(CScene* scene)
{
    shadowFrameBuffer->BindFBO();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    shadowBox.Update(scene->GetCamera());

    try
    {
        auto light_direction = scene->GetDirectionalLight().GetPosition() * -1.f;
        PrepareLightViewMatrix(light_direction);
    }
    catch (const std::exception& e)
    {
        e.what();
        return;
    }
}

void CShadowMapRenderer::PrepareLightViewMatrix(const glm::vec3& light_direction)
{
    auto projection_matrix = Utils::CreateOrthoProjectionMatrix(shadowBox.GetWidth(), shadowBox.GetHeight(), shadowBox.GetLength());
    auto lightViewMatrix = Utils::CreateLightViewMatrix(light_direction, shadowBox.GetCenter());
    shadowBox.SetLightViewMatrix(lightViewMatrix);
    projectionViewMatrix = projection_matrix * lightViewMatrix;
}

void CShadowMapRenderer::RenderSubscribes() const
{
    for (auto& sub : subscribes)
        RenderEntity(sub);
}

void CShadowMapRenderer::RenderEntity(CEntity* entity) const
{
    for (const CMesh& mesh : entity->GetModel(0)->GetMeshes())
        RenderMesh(mesh, entity->worldTransform.GetMatrix());
}

void CShadowMapRenderer::RenderMesh(const CMesh& mesh, const mat4 &transform_matrix) const
{
    if(!mesh.IsInit())
        return;

    Utils::EnableVao ev(mesh.GetVao(), {0, 1});

    BindMaterial(mesh.GetMaterial());

    shader.LoadUseBonesTransformation(0.f);
    shader.LoadUseInstancedRendering(0.f);

    auto transform_matrix_ = transform_matrix * mesh.GetMeshTransform();
    shader.LoadTransformMatrix(transform_matrix_);

    glDrawElements(GL_TRIANGLES, mesh.GetVertexCount(), GL_UNSIGNED_SHORT, 0);
}

void CShadowMapRenderer::BindMaterial(const SMaterial& material) const
{
    if(material.diffuseTexture == nullptr)
        return;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material.diffuseTexture->GetId());

}

void CShadowMapRenderer::PrepareShader(CCamera* camera) const
{
    shader.Start();
    shader.LoadProjectionMatrix(projectionViewMatrix);
    shader.LoadViewMatrix(camera->GetViewMatrix());
}

