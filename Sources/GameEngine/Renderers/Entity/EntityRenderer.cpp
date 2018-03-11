#include "EntityRenderer.h"
#include "../../Renderers/Projection.h"
#include "../../Scene/Scene.hpp"
#include "../../Objects/RenderAble/Entity/Entity.h"
#include "../Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"

#include "SingleTon.h"
#include "GameEngine/Engine/AplicationContext.h"

#include "OpenGL/OpenGLUtils.h"
#include "Logger/Log.h"

//Get Entity by reference
static std::list<CEntity*> sEmptyEntityList;

CEntityRenderer::CEntityRenderer(CProjection* projection_matrix, CFrameBuffer* framebuffer)
	: CRenderer(framebuffer)
	, projectionMatrix(projection_matrix)
	, clipPlane(vec4(0, 1, 0, 100000))
{
	subscribes.resize(gridSize * gridSize);
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

	auto index = CalcualteCoorditantes(scene->GetCamera()->GetPosition());
	
	target->BindToDraw();
	shader.Start();
	RenderDynamicsEntities();
	RenderStaticEntities(index);
	shader.Stop();
}

void CEntityRenderer::EndFrame(GameEngine::Scene* scene)
{
	shader.Stop();
}

void CEntityRenderer::Subscribe(CGameObject * gameObject)
{
	auto entity = dynamic_cast<CEntity*>(gameObject);

	if (entity == nullptr)
		return;

	if (entity->worldTransform.isDynamic_)
	{
		dynamicSubscribes.push_back(entity);
		return;
	}
	wb::vec2i index = CalcualteCoorditantes(entity->worldTransform.GetPosition());

	//if (xi < gridSize && xi >0)
	//	return;
	//if (yi > 0 && yi < yi * gridSize)
	//	return;

	subscribes[index.x + index.y*gridSize].push_back(entity);
}

void CEntityRenderer::UnSubscribe(CGameObject * gameObject)
{
	for (auto iter = dynamicSubscribes.begin(); iter != dynamicSubscribes.end(); ++iter)
	{
		if ((*iter)->GetId() == gameObject->GetId())
		{
			dynamicSubscribes.erase(iter);
			return;
		}
	}

	for (auto& list : subscribes)
	{
		for (auto iter = list.begin(); iter != list.end(); ++iter)
		{
			if ((*iter)->GetId() == gameObject->GetId())
			{
				list.erase(iter);
				return;
			}
		}
	}
}

void CEntityRenderer::UnSubscribeAll()
{
	dynamicSubscribes.clear();
	subscribes.clear();
	subscribes.resize(gridSize * gridSize);
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

const std::list<CEntity*>& CEntityRenderer::GetEntity(uint32 x, uint32 y) const
{
	if (subscribes.empty()) return sEmptyEntityList;

	if ((x + y * gridSize) > subscribes.size())
		return sEmptyEntityList;

	return subscribes[x + y * gridSize];
}

void CEntityRenderer::RenderModel(CModel * model, const mat4 & transform_matrix) const
{
	for (const auto& mesh : model->GetMeshes())
		RenderMesh(mesh, transform_matrix);
}

void CEntityRenderer::RenderMesh(const CMesh &mesh, const mat4 &transform_matrix) const
{
	Utils::EnableVao ev(mesh.GetVao(), mesh.GetUsedAttributes());
	BindMaterial(mesh.GetMaterial());

	auto transform_matrix_ = transform_matrix * mesh.GetMeshTransform();
	shader.LoadTransformMatrix(transform_matrix_);
	glDrawElements(GL_TRIANGLES, mesh.GetVertexCount(), GL_UNSIGNED_SHORT, 0);

	UnBindMaterial(mesh.GetMaterial());
}

void CEntityRenderer::RenderDynamicsEntities()
{
	for (auto& entity : dynamicSubscribes)
	{
		if (entity == nullptr)
			Log("[Error] Null subsciber in EnityRenderer.");
		if (entity->GetModel(GameEngine::LevelOfDetail::L1) == nullptr)
			continue;

		shader.LoadUseBonesTransformation(1.f);
		auto& mesh = entity->GetModel(GameEngine::LevelOfDetail::L1)->meshes.front();
		//for(int x = 0; x < 5; ++x)
		/*for (auto& transform : mesh.animator_.animations_.begin()->second.frames[0].idTransforms_)
			{
				mat4 t = transform.second.transform;
				auto j = mesh.rootJoint_.GetJoint(transform.first);
				if (j != nullptr)
					t = t * j->invTransform;

				shader.LoadBoneTransform(t, transform.first);
			}*/
		
		int x = 0;
		for (auto& t : mesh.GetJointTransforms())
		{
			shader.LoadBoneTransform(t, x++);
		}
		RenderModel(entity->GetModel(GameEngine::LevelOfDetail::L1), entity->worldTransform.GetMatrix());
		mesh.animator_.Update(&mesh, 0.1f);
		shader.LoadUseBonesTransformation(0.f);
	}
}

void CEntityRenderer::RenderStaticEntities(const wb::vec2i &index)
{
	for (int y = -2; y <= 2; y++)
	{
		for (int x = -2; x <= 2; x++)
		{
			auto sub = GetEntity(index.x + x, index.y + y);

			GameEngine::LevelOfDetail level_of_detail = GameEngine::LevelOfDetail::L1;

			if (y > 1 || y < -1 || x > 2 || x < -2)
				level_of_detail = GameEngine::LevelOfDetail::L2;
			else if (y > 0 || y < 0 || x > 0 || x < 0)
				level_of_detail = GameEngine::LevelOfDetail::L3;

			for (auto& entity : sub)
			{
				if (entity == nullptr)
					Log("[Error] Null subsciber in EnityRenderer.");

				auto model = entity->GetModel(level_of_detail);
				if (model == nullptr)
					continue;

				RenderModel(model, entity->worldTransform.GetMatrix());
			}
		}
	}
}

wb::vec2i CEntityRenderer::CalcualteCoorditantes(const vec3 & v) const
{
	wb::vec2i w;
	w.x = static_cast<int>(v.x) / static_cast<int>(gridCellSize) + gridSize / 2;
	w.y = static_cast<int>(v.z) / static_cast<int>(gridCellSize) + gridSize / 2;
	return w;
}

void ActiveTexture(int i, CTexture* texture)
{
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, texture->GetId());
}

void CEntityRenderer::BindMaterial(const SMaterial & material) const
{
	if (material.isTransparency)
		Utils::DisableCulling();

	shader.LoadMeshMaterial(material);

	if (material.diffuseTexture != nullptr && material.diffuseTexture->IsInitialized())
	{
		ActiveTexture(0, material.diffuseTexture);
		shader.LoadUseTexture(1.f);
	}
	else
		shader.LoadUseTexture(0.f);

	if (material.ambientTexture != nullptr && material.ambientTexture->IsInitialized())
	{
		ActiveTexture(1, material.ambientTexture);
	}

	if (material.normalTexture != nullptr && material.normalTexture->IsInitialized())
	{
		ActiveTexture(2, material.normalTexture);
		shader.LoadUseNormalMap(1.f);
	}
	else
		shader.LoadUseNormalMap(0.f);

	if (material.specularTexture != nullptr && material.specularTexture->IsInitialized())
		ActiveTexture(3, material.specularTexture);
}

void CEntityRenderer::UnBindMaterial(const SMaterial & material) const
{
	if (material.isTransparency)
		Utils::EnableCulling();
}
