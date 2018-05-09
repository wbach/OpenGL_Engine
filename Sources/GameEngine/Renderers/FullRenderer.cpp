#include "FullRenderer.h"
#include "LightPassRenderer.h"

#include "Objects/Tree/TreeRenderer.h"
#include "Objects/Particles/ParticlesRenderer.h"
#include "Objects/Plants/PlantsRenderer.h"
#include "Objects/Water/WaterRenderer.h"
#include "Objects/Grass/GrassRenderer.h"
#include "Objects/Entity/EntityRenderer.h"
#include "Objects/SkyBox/SkyBoxRenderer.h"
#include "Objects/Shadows/ShadowMapRenderer.hpp"
#include "Objects/Shadows/ShadowFrameBuffer.h"
#include "Objects/Terrain/TerrainRenderer.h"
#include "Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "Logger/Log.h"

using namespace GameEngine;

FullRenderer::FullRenderer(GameEngine::IGraphicsApiPtr graphicsApi, CProjection* projection_matrix)
	: graphicsApi_(graphicsApi)
{	
	rendererContext_.projectionMatrix_ = projection_matrix;
	rendererContext_.shadowsFrameBuffer_ = std::make_shared<CShadowFrameBuffer>(graphicsApi);
	rendererContext_.defferedFrameBuffer_ = std::make_shared<CDefferedFrameBuffer>(graphicsApi);

	CreateRenderers();
}

FullRenderer::~FullRenderer()
{
	Log(__FUNCTION__);
}

void FullRenderer::Init()
{
	rendererContext_.defferedFrameBuffer_->Init(rendererContext_.projectionMatrix_->GetWindowSize());
	rendererContext_.shadowsFrameBuffer_->InitialiseFrameBuffer();

    for(auto& renderer : renderers)
    {
        renderer->Init();
    }
	Log("FullRenderer initialized.");
}
void FullRenderer::PrepareFrame(GameEngine::Scene* scene)
{
	rendererContext_.defferedFrameBuffer_->Clean();

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

void FullRenderer::CreateRenderers()
{
	renderers.emplace_back(new CSkyBoxRenderer(rendererContext_));
	if (EngineConf.isShadows) renderers.emplace_back(new CShadowMapRenderer(graphicsApi_, rendererContext_.projectionMatrix_, &rendererContext_));
	if (EngineConf.advancedGrass) renderers.emplace_back(new CGrassRenderer(graphicsApi_, rendererContext_.projectionMatrix_, rendererContext_.defferedFrameBuffer_.get()));
	renderers.emplace_back(new CTerrainRenderer(graphicsApi_, rendererContext_.projectionMatrix_, rendererContext_.defferedFrameBuffer_.get(), &rendererContext_));
	renderers.emplace_back(new TreeRenderer(graphicsApi_, rendererContext_.projectionMatrix_, rendererContext_.defferedFrameBuffer_.get()));
	renderers.emplace_back(new PlantsRenderer(graphicsApi_, rendererContext_.projectionMatrix_, rendererContext_.defferedFrameBuffer_.get()));
	renderers.emplace_back(new CEntityRenderer(graphicsApi_, rendererContext_.projectionMatrix_, rendererContext_.defferedFrameBuffer_.get()));
	renderers.emplace_back(new WaterRenderer(graphicsApi_, rendererContext_.projectionMatrix_, rendererContext_.defferedFrameBuffer_.get()));
	if (EngineConf.useParticles) renderers.emplace_back(new ParticlesRenderer(graphicsApi_, rendererContext_.projectionMatrix_, rendererContext_.defferedFrameBuffer_.get()));
	renderers.emplace_back(new CLightPassRenderer(graphicsApi_, rendererContext_.projectionMatrix_, rendererContext_.defferedFrameBuffer_.get()));
}
