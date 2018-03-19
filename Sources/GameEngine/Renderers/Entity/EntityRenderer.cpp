#include "EntityRenderer.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GameEngine/Renderers/Projection.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Objects/RenderAble/Entity/Entity.h"
#include "../Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"
#include "GameEngine/Components/Renderer/RendererComponent.hpp"
#include "Logger/Log.h"

//Get Entity by reference
static SubscribersMap sEmptyEntityList;

CEntityRenderer::CEntityRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection_matrix, CFrameBuffer* framebuffer)
	: CRenderer(framebuffer)
	, graphicsApi_(graphicsApi)
	, shader(graphicsApi)
	, projectionMatrix(projection_matrix)
	, clipPlane(vec4(0, 1, 0, 100000))
{

}

void CEntityRenderer::Init()
{
	Log("Start initialize entity renderer...");
	shader.Init();
	shader.Start();
	assert(projectionMatrix != nullptr);
	shader.LoadViewDistance(500.f);
	shader.LoadProjectionMatrix(projectionMatrix->GetProjectionMatrix());
	shader.Stop();

	Log("CEntityRenderer initialized.");
}

void CEntityRenderer::PrepareFrame(GameEngine::Scene * scene)
{
	shader.Start();
	shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix());
	shader.LoadClipPlane(clipPlane);
	shader.LoadShadowValues(0.f, 10.f, 10.f);
	shader.LoadUseInstancedRendering(0.f);
	shader.LoadUseFakeLight(0.f);
	shader.LoadUseBonesTransformation(0.f);
	rendererObjectPerFrame = 0;
	rendererVertixesPerFrame = 0;
	shader.Stop();
}

void CEntityRenderer::Render(GameEngine::Scene * scene)
{
	if (target == nullptr)
		return;
	
	target->BindToDraw();
	shader.Start();
	RenderEntities();
	shader.Stop();
}

void CEntityRenderer::EndFrame(GameEngine::Scene* scene)
{
	shader.Stop();
}

void CEntityRenderer::Subscribe(CGameObject* gameObject)
{
	auto rendererComponent = gameObject->GetComponent<GameEngine::Components::RendererComponent>();

	if (rendererComponent == nullptr)
		return;

	subscribes_[gameObject->GetId()] = { gameObject, &rendererComponent->GetModelWrapper() };
}

void CEntityRenderer::UnSubscribe(CGameObject * gameObject)
{
	subscribes_.erase(gameObject->GetId());
}

void CEntityRenderer::UnSubscribeAll()
{
	subscribes_.clear();
}

void CEntityRenderer::ReloadShaders()
{
	shader.Reload();
	shader.Init();
	shader.Start();
	assert(projectionMatrix != nullptr);
	shader.LoadViewDistance(500.f);
	shader.LoadProjectionMatrix(projectionMatrix->GetProjectionMatrix());
	shader.Stop();
}

void CEntityRenderer::RenderModel(CModel * model, const mat4 & transform_matrix) const
{
	const auto& meshes = model->GetMeshes();
	for (const auto& mesh : meshes)
		RenderMesh(mesh, transform_matrix);
}

void CEntityRenderer::RenderMesh(const CMesh &mesh, const mat4 &transform_matrix) const
{
	BindMaterial(mesh.GetMaterial());
	auto transform_matrix_ = transform_matrix * mesh.GetMeshTransform();
	shader.LoadTransformMatrix(transform_matrix_);
	shader.LoadUseBonesTransformation(static_cast<float>(mesh.UseArmature()));
	graphicsApi_->RenderMesh(mesh.GetObjectId());
	UnBindMaterial(mesh.GetMaterial());
}

void CEntityRenderer::RenderEntities()
{
	for (auto& sub : subscribes_)
	{
		if (sub.second.gameObject == nullptr || sub.second.model == nullptr)
			Log("[Error] Null subsciber in EnityRenderer.");

		auto model = sub.second.model->Get(GameEngine::LevelOfDetail::L1);
		
		if (model == nullptr)
			continue;

		uint32 x = 0;
		for (auto& t : model->GetBoneTransforms())
			shader.LoadBoneTransform(t, x++);

		RenderModel(model, sub.second.gameObject->worldTransform.GetMatrix());
	}
}

void CEntityRenderer::BindMaterial(const SMaterial & material) const
{
	if (material.isTransparency)
		graphicsApi_->DisableCulling();

	shader.LoadMeshMaterial(material);

	if (material.diffuseTexture != nullptr && material.diffuseTexture->IsInitialized())
	{
		graphicsApi_->ActiveTexture(0, material.diffuseTexture->GetId());
		shader.LoadUseTexture(1.f);
	}
	else
		shader.LoadUseTexture(0.f);

	if (material.ambientTexture != nullptr && material.ambientTexture->IsInitialized())
	{
		graphicsApi_->ActiveTexture(1, material.ambientTexture->GetId());
	}

	if (material.normalTexture != nullptr && material.normalTexture->IsInitialized())
	{
		graphicsApi_->ActiveTexture(2, material.normalTexture->GetId());
		shader.LoadUseNormalMap(1.f);
	}
	else
		shader.LoadUseNormalMap(0.f);

	if (material.specularTexture != nullptr && material.specularTexture->IsInitialized())
		graphicsApi_->ActiveTexture(3, material.specularTexture->GetId());
}

void CEntityRenderer::UnBindMaterial(const SMaterial & material) const
{
	if (material.isTransparency)
		graphicsApi_->EnableCulling();
}
