#include "LightPassRenderer.h"
#include "Framebuffer/FrameBuffer.h"
#include "../Engine/Projection.h"
#include "../Scene/Scene.hpp"

CLightPassRenderer::CLightPassRenderer(CProjection * projection, std::weak_ptr<CFrameBuffer> frambuffer)
	: m_Projection(projection)
	, CRenderer(frambuffer)
{
}

void CLightPassRenderer::Init()
{
	m_Shader.Init();
	m_Shader.Start();
	m_Shader.LoadSkyColour(glm::vec3(0.0));
	m_Shader.LoadScreenSize(m_Projection->GetWindowSize());
	m_Shader.LoadViewDistance(m_Projection->GetViewDistance());
	m_Shader.Stop();

	m_Quad.Init();

	Log("CLightPassRenderer initialized.");
}

void CLightPassRenderer::PrepareFrame(CScene * scene)
{	
}

void CLightPassRenderer::Render(CScene * scene)
{	
	
}

void CLightPassRenderer::EndFrame(CScene * scene)
{
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	auto target = m_Target.lock();

	if (target == nullptr)
		return;

	target->BindTextures();
	target->UnBind();

	m_Shader.Start();
	m_Shader.LoadSkyColour(glm::vec3(0.8) /** scene->m_DayNightCycle.GetDayNightBlendFactor()*/);
	m_Shader.LoadCameraPosition(scene->GetCamera()->GetPosition());
	m_Shader.LoadLight(scene->GetDirectionalLight(), 0);
	int lights = scene->GetLights().size() + 1;
	m_Shader.LoadLightNumber(lights);
	int i = 1;
	for (const auto& light : scene->GetLights())
	{
		m_Shader.LoadLight(light, i++);
	}
	m_Quad.Render();
	m_Shader.Stop();
}
