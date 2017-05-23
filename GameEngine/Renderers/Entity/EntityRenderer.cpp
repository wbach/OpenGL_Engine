#include "EntityRenderer.h"
#include "../../Engine/Projection.h"
#include "../../Scene/Scene.hpp"
#include "../../Objects/RenderAble/Entity/Entity.h"
#include "../../Utils/OpenGL/OpenGLUtils.h"
#include "../Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"

//Get Entity by reference
static std::list<CEntity*> sEmptyEntityList;

CEntityRenderer::CEntityRenderer(CProjection* projection_matrix, CFrameBuffer* framebuffer)
    : CRenderer(framebuffer)
    , projectionMatrix(projection_matrix)
	, clipPlane(glm::vec4(0, 1, 0, 100000))
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

void CEntityRenderer::PrepareFrame(CScene * scene)
{
	shader.Start();
	shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix());
	shader.LoadClipPlane(clipPlane);
	shader.LoadShadowValues(0.f, 10.f, 10.f);

	rendererObjectPerFrame = 0;
	rendererVertixesPerFrame = 0;
	shader.Stop();
}

void CEntityRenderer::Render(CScene * scene)
{
	if (target == nullptr)
		return;
	target->BindToDraw();

	shader.Start();

	auto index = CalcualteCoorditantes(scene->GetCamera()->GetPosition());

	auto subscirbes = dynamicSubscribes;

	for (auto& entity : subscirbes)
	{
		if (entity == nullptr)
			Log("[Error] Null subsciber in EnityRenderer.");
		if (entity->GetModel(0) == nullptr)
			continue;

        RenderModel(entity->GetModel(0), entity->worldTransform.GetMatrix());
	}

	for (int y = -2; y <= 2; y++)
	{
		for (int x = -2; x <= 2; x++)
		{
			auto sub = GetEntity(index.x + x, index.y + y);

			uint level_of_detail = 0;

			if (y > 1 || y < -1 || x > 2 || x < -2)
				level_of_detail = 2;
			else if (y > 0 || y < 0 || x > 0 || x < 0)
				level_of_detail = 1;

			for (auto& entity : sub)
			{
				if (entity == nullptr)
					Log("[Error] Null subsciber in EnityRenderer.");

				auto model = entity->GetModel(level_of_detail);
				if (model == nullptr)
					continue;

                RenderModel(model, entity->worldTransform.GetMatrix());
			}
			//subscirbes.insert(subscirbes.end(), sub.begin(), sub.end());
		}
	}	
	shader.Stop();
}

void CEntityRenderer::EndFrame(CScene * scene)
{	
	shader.Stop();
}

void CEntityRenderer::Subscribe(CGameObject * gameObject)
{
	auto entity = dynamic_cast<CEntity*>(gameObject);

	if (entity == nullptr)
		return;

	if (entity->dynamic)
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

const std::list<CEntity*>& CEntityRenderer::GetEntity(uint x, uint y) const
{
	if (subscribes.empty()) return sEmptyEntityList;

	if ((x + y*gridSize) > subscribes.size())
		return sEmptyEntityList;

	return subscribes[x + y*gridSize];
}

void CEntityRenderer::RenderModel(CModel * model, const glm::mat4 & transform_matrix) const
{
	shader.LoadTransformMatrix(transform_matrix);	
	shader.LoadUseInstancedRendering(0.f);
	shader.LoadUseFakeLight(0.f);
	shader.LoadUseBonesTransformation(0.f);

	for (const auto& mesh : model->GetMeshes())
	{
		Utils::EnableVao(mesh.GetVao(), mesh.GetUsedAttributes());
		BindMaterial(mesh.GetMaterial());			

		glDrawElements(GL_TRIANGLES, mesh.GetVertexCount(), GL_UNSIGNED_SHORT, 0);
		Utils::DisableVao(mesh.GetUsedAttributes());

		UnBindMaterial(mesh.GetMaterial());
	}
}

wb::vec2i CEntityRenderer::CalcualteCoorditantes(const glm::vec3 & v) const
{
	wb::vec2i w;
	w.x = static_cast<int>(v.x) / static_cast<int>(gridCellSize) + gridSize / 2;
	w.y = static_cast<int>(v.z) / static_cast<int>(gridCellSize) + gridSize / 2;
	return w;
}

void CEntityRenderer::BindMaterial(const SMaterial & material) const
{
	if (material.isTransparency)
		Utils::DisableCulling();

	shader.LoadMeshMaterial(material);

	if (material.diffuseTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.diffuseTexture->GetId());	
		shader.LoadUseTexture(1.f);
	}
	else
		shader.LoadUseTexture(0.f);

	if (material.ambientTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.ambientTexture->GetId());
	}

	if (material.normalTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material.normalTexture->GetId());
		shader.LoadUseNormalMap(1.f);
	}
	else
		shader.LoadUseNormalMap(0.f);

	if (material.specularTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, material.specularTexture->GetId());
	}
}

void CEntityRenderer::UnBindMaterial(const SMaterial & material) const
{
    if (material.isTransparency)
        Utils::EnableCulling();
}
