#include "GrassRenderer.h"

#include "../Framebuffer/FrameBuffer.h"

#include "../../Scene/Scene.hpp"
#include "../../Engine/Projection.h"
#include "../../Engine/Configuration.h"
#include "../../Objects/RenderAble/Grass.h"

CGrassRenderer::CGrassRenderer(CProjection * projection_matrix, CFrameBuffer* framebuffer)
	: m_Projection(projection_matrix)
	, CRenderer(framebuffer)
{	
}

void CGrassRenderer::Init()
{	
	m_Shader.Init();
	m_Shader.Start();
	m_Shader.LoadProjectionMatrix(m_Projection->GetProjectionMatrix());
	m_Shader.Stop();

	viewDistance = SConfiguration::Instance().floraViewDistance;
	Log("Grass renderer initialized.");
}

void CGrassRenderer::PrepareFrame(CScene * scene)
{		
}

void CGrassRenderer::Render(CScene * scene)
{
	if (m_Target == nullptr)
		return;
	m_Target->BindToDraw();

	m_Shader.Start();
	m_Shader.LoadGlobalTime(scene->GetGlobalTime());
	m_Shader.LoadShadowValues(0.f, 0.f, 512.f);
	m_Shader.LoadViewMatrix(scene->GetCamera()->GetViewMatrix());
    m_Shader.LoadViewDistance(viewDistance);

	Utils::DisableCulling();

	glActiveTexture(GL_TEXTURE0);	

	for (const auto& s : m_Subscribes)
	{
		if (s->model == nullptr)
			continue;

		for (const auto& mesh : s->model->GetMeshes())
		{
			if (mesh.GetMaterial().m_DiffuseTexture == nullptr)
				continue;

			glBindTexture(GL_TEXTURE_2D, mesh.GetMaterial().m_DiffuseTexture->GetId());
			glBindVertexArray(mesh.GetVao());
			glEnableVertexAttribArray(0);
			glDrawArrays(GL_POINTS, 0, mesh.GetVertexCount());
			glDisableVertexAttribArray(0);
			glBindVertexArray(0);
		}
	}
	Utils::EnableCulling();
	
	m_Shader.Stop();
}

void CGrassRenderer::EndFrame(CScene * scene)
{
}

void CGrassRenderer::Subscribe(CGameObject * gameObject)
{
	auto grass = dynamic_cast<SGrass*>(gameObject);
	if (grass != nullptr)
		m_Subscribes.push_back(grass);
}
