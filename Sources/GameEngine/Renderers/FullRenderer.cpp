#include "FullRenderer.h"
#include "LightPassRenderer.h"

#include "Tree/TreeRenderer.h"
#include "Particles/ParticlesRenderer.h"
#include "Plants/PlantsRenderer.h"
#include "Water/WaterRenderer.h"
#include "Grass/GrassRenderer.h"
#include "Entity/EntityRenderer.h"
#include "SkyBox/SkyBoxRenderer.h"
#include "Shadows/ShadowMapRenderer.hpp"
#include "Shadows/ShadowFrameBuffer.h"
#include "Terrain/TerrainRenderer.h"
#include "Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"

#include "../Engine/Configuration.h"
#include "../Renderers/Projection.h"
#include "Logger/Log.h"

using namespace GameEngine;

FullRenderer::FullRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection_matrix)
	: graphicsApi_(graphicsApi)
	, projectionMatrix(projection_matrix)
	, defferedFrameBuffer(new CDefferedFrameBuffer(graphicsApi))
{	
	rendererContext_.shadowsFrameBuffer = std::make_shared<CShadowFrameBuffer>(graphicsApi_);

	renderers.emplace_back(new CSkyBoxRenderer(graphicsApi_, projection_matrix, defferedFrameBuffer.get()));
    if(EngineConf.isShadows) renderers.emplace_back(new CShadowMapRenderer(graphicsApi_, projection_matrix, &rendererContext_));
    if(EngineConf.advancedGrass) renderers.emplace_back(new CGrassRenderer(graphicsApi_, projection_matrix, defferedFrameBuffer.get()));
    renderers.emplace_back(new CTerrainRenderer(graphicsApi_, projection_matrix, defferedFrameBuffer.get(), &rendererContext_));
	renderers.emplace_back(new TreeRenderer(graphicsApi_, projectionMatrix, defferedFrameBuffer.get()));
	renderers.emplace_back(new PlantsRenderer(graphicsApi_, projectionMatrix, defferedFrameBuffer.get()));
	renderers.emplace_back(new CEntityRenderer(graphicsApi_, projection_matrix, defferedFrameBuffer.get()));
	renderers.emplace_back(new WaterRenderer(graphicsApi_, projectionMatrix, defferedFrameBuffer.get()));
	if(EngineConf.useParticles) renderers.emplace_back(new ParticlesRenderer(graphicsApi_, projectionMatrix, defferedFrameBuffer.get()));
	renderers.emplace_back(new CLightPassRenderer(graphicsApi_, projection_matrix, defferedFrameBuffer.get()));
}

FullRenderer::~FullRenderer()
{
	Log(__FUNCTION__);
}

void FullRenderer::Init()
{
	defferedFrameBuffer->Init(projectionMatrix->GetWindowSize());
	rendererContext_.shadowsFrameBuffer->InitialiseFrameBuffer();

    for(auto& renderer : renderers)
    {
        renderer->Init();
    }
	Log("FullRenderer initialized.");
}
void FullRenderer::PrepareFrame(GameEngine::Scene* scene)
{
	defferedFrameBuffer->Clean();

    for(auto& renderer : renderers)
    {
        renderer->PrepareFrame(scene);
    }
}
void FullRenderer::Render(GameEngine::Scene* scene)
{
    for(auto& renderer : renderers)
    {
        renderer->Render(scene);
    }
}
void FullRenderer::EndFrame(GameEngine::Scene* scene)
{
    for(auto& renderer : renderers)
    {
        renderer->EndFrame(scene);
    }
}

void FullRenderer::Subscribe(CGameObject* gameObject)
{
	for (auto& renderer : renderers)
		renderer->Subscribe(gameObject);
}

void FullRenderer::UnSubscribe(CGameObject* gameObject)
{
	for (auto& r : renderers)
		r->UnSubscribe(gameObject);
}

void FullRenderer::UnSubscribeAll()
{
	for (auto& r : renderers)
		r->UnSubscribeAll();
}

void FullRenderer::ReloadShaders()
{
	for (auto& renderer : renderers)
		renderer->ReloadShaders();
}
