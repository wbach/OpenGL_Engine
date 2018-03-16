#include "ShadowMapRenderer.hpp"
#include "ShadowFrameBuffer.h"

#include "../../Engine/Configuration.h"
#include "../../Renderers/Projection.h"
#include "../../Camera/Camera.h"
#include "../../Objects/RenderAble/Entity/Entity.h"
#include "GameEngine/Components/Renderer/RendererComponent.hpp"
#include "GameEngine/Resources/Models/ModelWrapper.h"

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
	auto rendererComponent = gameObject->GetComponent<GameEngine::Components::RendererComponent>();

	if (rendererComponent == nullptr)
		return;

	subscribes_[gameObject->GetId()] = { gameObject, &rendererComponent->GetModelWrapper() };
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
	for (auto& sub : subscribes_)
		RenderSubscriber(sub.second);
}

void CShadowMapRenderer::RenderSubscriber(const Subscriber& sub) const
{
	auto model = sub.model->Get(GameEngine::LevelOfDetail::L1);

	if (model == nullptr)
		return;

	int x = 0;
	for (auto& t : model->GetBoneTransforms())
	{
		shader.LoadBoneTransform(t, x++);
	}

	const auto& meshes = model->GetMeshes();

	for (const CMesh& mesh : meshes)
		RenderMesh(mesh, sub.gameObject->worldTransform.GetMatrix());
}

void CShadowMapRenderer::RenderMesh(const CMesh& mesh, const mat4 &transform_matrix) const
{
	if (!mesh.IsInit())
		return;

	Utils::EnableVao ev(mesh.GetVao(), mesh.GetUsedAttributes(), { VertexBufferObjects::NORMAL, VertexBufferObjects::TANGENT });

	BindMaterial(mesh.GetMaterial());

	shader.LoadUseBonesTransformation(static_cast<float>(mesh.UseArmature()));

	auto transform_matrix_ = transform_matrix * mesh.GetMeshTransform();
	shader.LoadTransformMatrix(transform_matrix_);

	glDrawElements(GL_TRIANGLES, mesh.GetVertexCount(), GL_UNSIGNED_SHORT, 0);

	shader.LoadUseBonesTransformation(0.f);
}

void CShadowMapRenderer::BindMaterial(const SMaterial& material) const
{
	if (material.diffuseTexture == nullptr)
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

