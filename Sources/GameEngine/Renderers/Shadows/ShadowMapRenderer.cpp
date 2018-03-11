#include "ShadowMapRenderer.hpp"
#include "ShadowFrameBuffer.h"

#include "../../Engine/Configuration.h"
#include "../../Renderers/Projection.h"
#include "../../Camera/Camera.h"
#include "../../Objects/RenderAble/Entity/Entity.h"

#include "GLM/GLMUtils.h"
#include "OpenGL/OpenGLUtils.h"
#include "math.hpp"
#include "Logger/Log.h"


CShadowMapRenderer::CShadowMapRenderer(CProjection* projection, GameEngine::RendererContext* rendererContext)
: shadowBox(projection)
, shadowBox2(projection)
, projection(projection)
, rendererContext_(rendererContext)
, projectionViewMatrix(1.f)
, viewOffset(Utils::CreateOffset())
{
}

void CShadowMapRenderer::Init()
{
    shader.Init();
	EngineConf.texturesIds["shadowMap"] = rendererContext_->shadowsFrameBuffer->GetShadowMap();
}

void CShadowMapRenderer::PrepareFrame(GameEngine::Scene* scene)
{

}

void CShadowMapRenderer::Render(GameEngine::Scene* scene)
{
    PrepareRender(scene);
    PrepareShader(scene->GetCamera());
    RenderSubscribes();
    shader.Stop();
	rendererContext_->shadowsFrameBuffer->UnbindFrameBuffer();
}

void CShadowMapRenderer::EndFrame(GameEngine::Scene* scene)
{

}

void CShadowMapRenderer::Subscribe(CGameObject* gameObject)
{
    auto entity = dynamic_cast<CEntity*>(gameObject);

    if (entity == nullptr)
        return;

    subscribes.push_back((entity));
}

void CShadowMapRenderer::PrepareRender(GameEngine::Scene* scene)
{
	rendererContext_->shadowsFrameBuffer->BindFBO();
    glEnable(GL_DEPTH_TEST);
    glClear(GL_DEPTH_BUFFER_BIT);
    shadowBox.Update(scene->GetCamera());

	auto cameraPos = scene->GetCamera()->GetPosition();
	scene->GetCamera()->SetPosition(vec3(0, 0, 0));
	shadowBox2.Update(scene->GetCamera());
	scene->GetCamera()->SetPosition(cameraPos);


	auto light_direction = scene->GetDirectionalLight().GetDirection();
	shadowBox.CalculateMatrixes(light_direction);
	shadowBox2.CalculateMatrixes(light_direction);


	rendererContext_->toShadowMapZeroMatrix_ = viewOffset * shadowBox2.GetProjectionViewMatrix();
}

void CShadowMapRenderer::RenderSubscribes() const
{
    for (auto& sub : subscribes)
        RenderEntity(sub);
}

void CShadowMapRenderer::RenderEntity(CEntity* entity) const
{
    for (const CMesh& mesh : entity->GetModel(GameEngine::LevelOfDetail::L1)->GetMeshes())
        RenderMesh(mesh, entity->worldTransform.GetMatrix());
}

void CShadowMapRenderer::RenderMesh(const CMesh& mesh, const mat4 &transform_matrix) const
{
    if(!mesh.IsInit())
        return;

	Utils::EnableVao ev(mesh.GetVao(), { {VertexBufferObjects::POSITION, 0}, {VertexBufferObjects::TEXT_COORD, 1}});

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
    shader.LoadProjectionMatrix(shadowBox.GetProjectionViewMatrix());
    shader.LoadViewMatrix(camera->GetViewMatrix());
}

