#include "LightPassRenderer.h"
#include "Framebuffer/FrameBuffer.h"
#include "../Renderers/Projection.h"
#include "../Scene/Scene.hpp"
#include "Logger/Log.h"

CLightPassRenderer::CLightPassRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection * projection, CFrameBuffer* frambuffer)
    : CRenderer(frambuffer)
	, graphicsApi_(graphicsApi)
	, shader(graphicsApi)
    , projection(projection)
{
}

void CLightPassRenderer::Init()
{
	shader.Init();
	shader.Start();
	shader.LoadSkyColour(vec3(0.0));
	shader.LoadScreenSize(projection->GetWindowSize());
	shader.LoadViewDistance(projection->GetViewDistance());
	shader.Stop();

	Log("CLightPassRenderer initialized.");
}

void CLightPassRenderer::PrepareFrame(GameEngine::Scene* scene)
{
}

void CLightPassRenderer::Render(GameEngine::Scene* scene)
{
}

void CLightPassRenderer::EndFrame(GameEngine::Scene* scene)
{
	graphicsApi_->PolygonModeRender();
	graphicsApi_->DisableDepthMask();
	graphicsApi_->DisableDepthTest();
	graphicsApi_->EnableBlend();

	if (target == nullptr)
		return;

	target->BindTextures();
	target->UnBind();

	shader.Start();
	shader.LoadSkyColour(vec3(0.8) /** scene->m_DayNightCycle.GetDayNightBlendFactor()*/);
	shader.LoadCameraPosition(scene->GetCamera()->GetPosition());
	shader.LoadLight(scene->GetDirectionalLight(), 0);
	int lights = scene->GetLights().size() + 1;
	shader.LoadLightNumber(lights);
	int i = 1;
	for (const auto& light : scene->GetLights())
	{
		shader.LoadLight(light, i++);
	}
	graphicsApi_->RenderQuad();
	shader.Stop();
}
