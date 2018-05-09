#include "ShadowMapRenderer.hpp"
#include "ShadowFrameBuffer.h"

#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Objects/RenderAble/Entity/Entity.h"
#include "GameEngine/Components/Renderer/RendererComponent.hpp"
#include "GLM/GLMUtils.h"
#include "Logger/Log.h"
#include "math.hpp"

CShadowMapRenderer::CShadowMapRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection, GameEngine::RendererContext* rendererContext)
	: graphicsApi_(graphicsApi)
	, projection(projection)
	, rendererContext_(rendererContext)
	, shader(graphicsApi)
	, shadowBox(projection)
	, shadowBox2(projection)
	, projectionViewMatrix(1.f)
	, viewOffset(Utils::CreateOffset())
{
}

void CShadowMapRenderer::Init()
{
	shader.Init();
	EngineConf.texturesIds["shadowMap"] = rendererContext_->shadowsFrameBuffer_->GetShadowMap();
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
	rendererContext_->shadowsFrameBuffer_->UnbindFrameBuffer();
}

void CShadowMapRenderer::EndFrame(GameEngine::Scene* scene)
{
}

void CShadowMapRenderer::Subscribe(CGameObject* gameObject)
{
	auto rendererComponent = gameObject->GetComponent<GameEngine::Components::RendererComponent>();

	if (rendererComponent == nullptr)
		return;

	subscribes_[gameObject->GetId()] = { rendererComponent->textureIndex, gameObject, &rendererComponent->GetModelWrapper() };
}

void CShadowMapRenderer::ReloadShaders()
{
	shader.Stop();
	shader.Reload();
	shader.Init();
}

void CShadowMapRenderer::PrepareRender(GameEngine::Scene* scene)
{
	rendererContext_->shadowsFrameBuffer_->BindFBO();
	graphicsApi_->EnableDepthTest();
	graphicsApi_->ClearBuffer(GameEngine::BufferType::DEPTH);
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
		shader.Load(CShadowShader::UniformLocation::BonesTransforms, *t, x++);
	}

	const auto& meshes = model->GetMeshes();

	for (const CMesh& mesh : meshes)
		RenderMesh(mesh, sub.gameObject->worldTransform.GetMatrix(), sub.textureIndex);
}

void CShadowMapRenderer::RenderMesh(const CMesh& mesh, const mat4 &transform_matrix, uint32 textureIndex) const
{
	if (!mesh.IsInit())
		return;

	auto transform_matrix_ = transform_matrix * mesh.GetMeshTransform();
	BindMaterial(mesh.GetMaterial(), textureIndex);
	shader.Load(CShadowShader::UniformLocation::UseBoneTransform, (static_cast<float>(mesh.UseArmature())));
	shader.Load(CShadowShader::UniformLocation::TransformationMatrix, transform_matrix_);

	graphicsApi_->RenderMesh(mesh.GetObjectId());
}

void CShadowMapRenderer::BindMaterial(const SMaterial& material, uint32 textureIndex) const
{
	if (material.diffuseTexture == nullptr)
		return;

	shader.Load(CShadowShader::UniformLocation::NumberOfRows, static_cast<float>(material.diffuseTexture->numberOfRows));
	shader.Load(CShadowShader::UniformLocation::TextureOffset, material.diffuseTexture->GetTextureOffset(textureIndex));
	graphicsApi_->ActiveTexture(0, material.diffuseTexture->GetId());
}

void CShadowMapRenderer::PrepareShader(GameEngine::ICamera* camera) const
{
	shader.Start();
	shader.Load(CShadowShader::UniformLocation::ProjectionViewMatrix, shadowBox.GetProjectionViewMatrix());
}

