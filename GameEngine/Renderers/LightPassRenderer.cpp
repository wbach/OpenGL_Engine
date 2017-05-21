#include "LightPassRenderer.h"
#include "Framebuffer/FrameBuffer.h"
#include "../Engine/Projection.h"
#include "../Scene/Scene.hpp"

CLightPassRenderer::CLightPassRenderer(CProjection * projection, CFrameBuffer* frambuffer)
    : CRenderer(frambuffer)
    , projection(projection)
{
}

void CLightPassRenderer::Init()
{
	shader.Init();
	shader.Start();
	shader.LoadSkyColour(glm::vec3(0.0));
	shader.LoadScreenSize(projection->GetWindowSize());
	shader.LoadViewDistance(projection->GetViewDistance());
	shader.Stop();

	quad.Init();

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

	if (target == nullptr)
		return;

	target->BindTextures();
	target->UnBind();

	shader.Start();
	shader.LoadSkyColour(glm::vec3(0.8) /** scene->m_DayNightCycle.GetDayNightBlendFactor()*/);
	shader.LoadCameraPosition(scene->GetCamera()->GetPosition());
	shader.LoadLight(scene->GetDirectionalLight(), 0);
	int lights = scene->GetLights().size() + 1;
	shader.LoadLightNumber(lights);
	int i = 1;
	for (const auto& light : scene->GetLights())
	{
		shader.LoadLight(light, i++);
	}
	quad.Render();
	shader.Stop();
}
