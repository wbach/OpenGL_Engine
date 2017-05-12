#include "SimpleRenderer.h"

SimpleRenderer::SimpleRenderer(SProjection* projection_matrix)
	: m_ProjectionMatrix(projection_matrix)
{
}

void SimpleRenderer::Init()
{
	m_Shader.Init();
	m_Shader.Start();
	assert(m_ProjectionMatrix != nullptr);
	m_Shader.LoadProjectionMatrix(m_ProjectionMatrix->GetProjectionMatrix());
	m_Shader.Stop();
}

void SimpleRenderer::PrepareFrame(CScene * scene)
{
	m_Shader.Start();
	m_Shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix());
}

void SimpleRenderer::Render(CScene * scene)
{
	for (auto& entity : m_Subscribes)
	{
		if (entity->GetModel(0) == nullptr)
			continue;

		RenderModel(entity->GetModel(0), entity->m_WorldTransform.GetMatrix());
	}
}

void SimpleRenderer::EndFrame(CScene * scene)
{
	m_Shader.Stop();
}

void SimpleRenderer::Subscribe(CGameObject * gameObject)
{
	auto entity = dynamic_cast<CEntity*>(gameObject);
	if (entity != nullptr)
		m_Subscribes.push_back(entity);
}

void SimpleRenderer::RenderModel(CModel * model, const glm::mat4 & transform_matrix) const
{
	m_Shader.LoadTransformMatrix(transform_matrix);

	for (const auto& mesh : model->GetMeshes())
	{
		Utils::EnableVao(mesh.GetVao(), mesh.GetUsedAttributes());
		BindTextures(mesh.GetMaterial());
		glDrawElements(GL_TRIANGLES, mesh.GetVertexCount(), GL_UNSIGNED_SHORT, 0);
		Utils::DisableVao(mesh.GetUsedAttributes());
	}
}

void SimpleRenderer::BindTextures(const SMaterial & material) const
{
	if (material.m_DiffuseTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.m_DiffuseTexture->GetId());	
			//TODO : load using texture to shader
			//m_SimpleShader.LOad
	}
}
