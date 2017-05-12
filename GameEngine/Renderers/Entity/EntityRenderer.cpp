#include "EntityRenderer.h"

CEntityRenderer::CEntityRenderer(SProjection* projection_matrix, std::weak_ptr<CFrameBuffer> framebuffer)
	: m_ProjectionMatrix(projection_matrix)
	, m_ClipPlane(glm::vec4(0, 1, 0, 100000))
	, CRenderer(framebuffer)
{
	m_Subscribes.resize(gridSize * gridSize);
}

void CEntityRenderer::Init()
{
    Log("Start initialize entity renderer...");
	m_Shader.Init();
	m_Shader.Start();
	assert(m_ProjectionMatrix != nullptr);
	m_Shader.LoadViewDistance(500.f);
	m_Shader.LoadProjectionMatrix(m_ProjectionMatrix->GetProjectionMatrix());
	m_Shader.Stop();

	Log("CEntityRenderer initialized.");
}

void CEntityRenderer::PrepareFrame(CScene * scene)
{
	m_Shader.Start();
	m_Shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix());
	m_Shader.LoadClipPlane(m_ClipPlane);
	m_Shader.LoadShadowValues(0.f, 10.f, 10.f);

	m_RendererObjectPerFrame = 0;
	m_RendererVertixesPerFrame = 0;
	m_Shader.Stop();
}

void CEntityRenderer::Render(CScene * scene)
{
	auto target = m_Target.lock();
	if (!target)
		return;
	target->BindToDraw();

	m_Shader.Start();

	auto index = CalcualteCoorditantes(scene->GetCamera()->GetPosition());

	auto subscirbes = dynamicSubscribes;

	for (auto& entity : subscirbes)
	{
		if (entity == nullptr)
			Log("[Error] Null subsciber in EnityRenderer.");
		if (entity->GetModel(0) == nullptr)
			continue;

		RenderModel(entity->GetModel(0), entity->m_WorldTransform.GetMatrix());
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

				RenderModel(model, entity->m_WorldTransform.GetMatrix());
			}


			//subscirbes.insert(subscirbes.end(), sub.begin(), sub.end());
		}
	}

	
	m_Shader.Stop();
}

void CEntityRenderer::EndFrame(CScene * scene)
{	
	m_Shader.Stop();
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
	wb::vec2i index = CalcualteCoorditantes(entity->m_WorldTransform.GetPosition());

	//if (xi < gridSize && xi >0)
	//	return;
	//if (yi > 0 && yi < yi * gridSize)
	//	return;

	m_Subscribes[index.x + index.y*gridSize].push_back(entity);
}

const std::list<CEntity*>& CEntityRenderer::GetEntity(uint x, uint y) const
{
	if (m_Subscribes.empty()) return std::list<CEntity*>();

	if ((x + y*gridSize) > m_Subscribes.size())
		return std::list<CEntity*>();

	return m_Subscribes[x + y*gridSize];
}

void CEntityRenderer::RenderModel(CModel * model, const glm::mat4 & transform_matrix) const
{
	m_Shader.LoadTransformMatrix(transform_matrix);	
	m_Shader.LoadUseInstancedRendering(0.f);
	m_Shader.LoadUseFakeLight(0.f);
	m_Shader.LoadUseBonesTransformation(0.f);

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

	m_Shader.LoadMeshMaterial(material);

	if (material.m_DiffuseTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.m_DiffuseTexture->GetId());	
		m_Shader.LoadUseTexture(1.f);
	}
	else
		m_Shader.LoadUseTexture(0.f);

	if (material.m_AmbientTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material.m_AmbientTexture->GetId());
	}

	if (material.m_NormalTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, material.m_NormalTexture->GetId());
		m_Shader.LoadUseNormalMap(1.f);
	}
	else
		m_Shader.LoadUseNormalMap(0.f);

	if (material.m_SpecularTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, material.m_SpecularTexture->GetId());
	}
}

void CEntityRenderer::UnBindMaterial(const SMaterial & material) const
{
    if (material.isTransparency)
        Utils::EnableCulling();
}
