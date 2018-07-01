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
namespace GameEngine
{
ShadowMapRenderer::ShadowMapRenderer(RendererContext& context)
	: context_(context)
	, shader_(context.graphicsApi_)
	, shadowBox_(context.projection_)
	, shadowBox2_(context.projection_)
	, projectionViewMatrix_(1.f)
	, viewOffset_(Utils::CreateOffset())
{
	__RegisterRenderFunction__(RendererFunctionType::CONFIGURE, ShadowMapRenderer::Render);
}

void ShadowMapRenderer::Init()
{
	shader_.Init();
}

void ShadowMapRenderer::Render(Scene* scene)
{
	PrepareRender(scene);
	PrepareShader(scene->GetCamera());
	RenderSubscribes();
	shader_.Stop();
	context_.shadowsFrameBuffer_->UnbindFrameBuffer();
}

void ShadowMapRenderer::Subscribe(CGameObject* gameObject)
{
	auto rendererComponent = gameObject->GetComponent<Components::RendererComponent>();

	if (rendererComponent == nullptr)
		return;

	subscribes_[gameObject->GetId()] = { rendererComponent->textureIndex, gameObject, &rendererComponent->GetModelWrapper() };
}

void ShadowMapRenderer::ReloadShaders()
{
	shader_.Stop();
	shader_.Reload();
	shader_.Init();
}

void ShadowMapRenderer::PrepareRender(Scene* scene)
{
	context_.shadowsFrameBuffer_->BindFBO();
	context_.graphicsApi_->EnableDepthTest();
	context_.graphicsApi_->ClearBuffer(BufferType::DEPTH);
	shadowBox_.Update(scene->GetCamera());

	auto cameraPos = scene->GetCamera()->GetPosition();
	scene->GetCamera()->SetPosition(vec3(0, 0, 0));
	shadowBox2_.Update(scene->GetCamera());
	scene->GetCamera()->SetPosition(cameraPos);


	auto light_direction = scene->GetDirectionalLight().GetDirection();
	shadowBox_.CalculateMatrixes(light_direction);
	shadowBox2_.CalculateMatrixes(light_direction);

	context_.toShadowMapZeroMatrix_ = viewOffset_ * shadowBox2_.GetProjectionViewMatrix();
}

void ShadowMapRenderer::RenderSubscribes() const
{
	for (auto& sub : subscribes_)
		RenderSubscriber(sub.second);
}

void ShadowMapRenderer::RenderSubscriber(const ShadowMapSubscriber& sub) const
{
	auto model = sub.model->Get(LevelOfDetail::L1);

	if (model == nullptr)
		return;

	int x = 0;
	for (auto& t : model->GetBoneTransforms())
	{
		shader_.Load(CShadowShader::UniformLocation::BonesTransforms, *t, x++);
	}

	const auto& meshes = model->GetMeshes();

	for (const CMesh& mesh : meshes)
		RenderMesh(mesh, sub.gameObject->worldTransform.GetMatrix(), sub.textureIndex);
}

void ShadowMapRenderer::RenderMesh(const CMesh& mesh, const mat4 &transform_matrix, uint32 textureIndex) const
{
	if (!mesh.IsInit())
		return;

	auto transform_matrix_ = transform_matrix * mesh.GetMeshTransform();
	BindMaterial(mesh.GetMaterial(), textureIndex);
	shader_.Load(CShadowShader::UniformLocation::UseBoneTransform, (static_cast<float>(mesh.UseArmature())));
	shader_.Load(CShadowShader::UniformLocation::TransformationMatrix, transform_matrix_);

	context_.graphicsApi_->RenderMesh(mesh.GetObjectId());
}

void ShadowMapRenderer::BindMaterial(const SMaterial& material, uint32 textureIndex) const
{
	if (material.diffuseTexture == nullptr)
		return;

	shader_.Load(CShadowShader::UniformLocation::NumberOfRows, static_cast<float>(material.diffuseTexture->numberOfRows));
	shader_.Load(CShadowShader::UniformLocation::TextureOffset, material.diffuseTexture->GetTextureOffset(textureIndex));
	context_.graphicsApi_->ActiveTexture(0, material.diffuseTexture->GetId());
}

void ShadowMapRenderer::PrepareShader(ICamera* camera) const
{
	shader_.Start();
	shader_.Load(CShadowShader::UniformLocation::ProjectionViewMatrix, shadowBox_.GetProjectionViewMatrix());
}
} // GameEngine
