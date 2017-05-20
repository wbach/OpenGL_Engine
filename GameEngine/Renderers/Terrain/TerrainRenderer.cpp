#include "TerrainRenderer.h"
#include "../Framebuffer/FrameBuffer.h"
#include "../../Scene/Scene.hpp"
#include "../../Engine/Projection.h"
#include "../../Objects/RenderAble/Terrain.h"
#include "../../Utils/GLM/GLMUtils.h"

const float heightFactor = 25.f;

CTerrainRenderer::CTerrainRenderer(CProjection * projection_matrix, CFrameBuffer* framebuffer)
	: m_ProjectionMatrix(projection_matrix)
	, m_ClipPlane(glm::vec4(0, 1, 0, 100000))
	, CRenderer(framebuffer)
{
}

void CTerrainRenderer::Init()
{
	m_Shader.Init();
	m_Shader.Start();
	assert(m_ProjectionMatrix != nullptr);

	GLfloat viewport[4];
	glGetFloatv(GL_VIEWPORT, viewport);
	m_Shader.Load(CTerrainShader::UniformLocation::Viewport, glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]));
	m_Shader.Load(CTerrainShader::UniformLocation::ViewDistance, 500.f);
	m_Shader.Load(CTerrainShader::UniformLocation::ProjectionMatrix, m_ProjectionMatrix->GetProjectionMatrix());
	m_Shader.Load(CTerrainShader::UniformLocation::HeightFactor, heightFactor);
	m_Shader.Stop();	

	Log("CTerrainRenderer initialized.");
}

void CTerrainRenderer::PrepareFrame(CScene * scene)
{
	for (auto& sub : m_Subscribes)
	{
		sub->m_Quad.Init();
	}	

	m_Shader.Start();
	m_Shader.Load(CTerrainShader::UniformLocation::ViewMatrix, scene->GetCamera()->GetViewMatrix());
	//m_Shader.Load(CTerrainShader::UniformLocation::ScreenSize, glm::vec2(1000, 600));
	
	//m_Shader.Load(CTerrainShader::UniformLocation::LodFactor, 4);
	//m_Shader.LoadClipPlane(m_ClipPlane);
	//m_Shader.LoadShadowValues(0.f, 10.f, 10.f);

	m_RendererObjectPerFrame = 0;
	m_RendererVertixesPerFrame = 0;
	m_Shader.Stop();
}

void CTerrainRenderer::Render(CScene * scene)
{
	if (m_Target == nullptr)
		return;

	m_Target->BindToDraw();
	m_Shader.Start();

	for (auto& sub : m_Subscribes)
	{
		auto position = sub->m_WorldTransform.GetPosition();
		position *= glm::vec3(100, -heightFactor / 2.f, 100);
		m_Shader.Load(CTerrainShader::UniformLocation::TransformMatrix, Utils::CreateTransformationMatrix(position, glm::vec3(0, 0, 0), glm::vec3(100)));

		BindTextures(sub);

		if (sub->model != nullptr)
		{
			for (auto& m : sub->model->GetMeshes())
			{			
				Utils::EnableVao(m.GetVao(), m.GetUsedAttributes());
                glDrawElements(GL_PATCHES, m.GetVertexCount(), GL_UNSIGNED_SHORT, 0);
				Utils::DisableVao(m.GetUsedAttributes());
			}				
		}
	}
	m_Shader.Stop();
}

void CTerrainRenderer::EndFrame(CScene * scene)
{
}

void CTerrainRenderer::Subscribe(CGameObject * gameObject)
{
	auto terrain = dynamic_cast<STerrain*>(gameObject);
	if (terrain != nullptr)
		m_Subscribes.push_back(terrain);
}

void CTerrainRenderer::RenderModel(CModel * model, const glm::mat4 & transform_matrix) const
{
}

void CTerrainRenderer::BindTextures(STerrain * terrain) const
{
	int i = 0;
	for (auto& t : terrain->m_Textures)
	{
		if (t != nullptr)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, t->GetId());
		}
		i++;
	}
}
