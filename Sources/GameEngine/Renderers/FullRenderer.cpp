#include "FullRenderer.h"


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

namespace GameEngine
{
FullRenderer::FullRenderer(IGraphicsApiPtr graphicsApi, CProjection* projection, std::function<void(RendererFunctionType, RendererFunction)> rendererFunction)
	: context_(projection, graphicsApi, std::make_shared<CDefferedFrameBuffer>(graphicsApi), std::make_shared<CShadowFrameBuffer>(graphicsApi), rendererFunction)
	, postprocessingRenderersManager_(context_)
{	
	CreateRenderers();
	__RegisterRenderFunction__(RendererFunctionType::PRECONFIGURE, FullRenderer::Prepare);
	__RegisterRenderFunction__(RendererFunctionType::ONENDFRAME, FullRenderer::PostProcess);
}

FullRenderer::~FullRenderer()
{
	Log(__FUNCTION__);
}

void FullRenderer::Init()
{
	context_.defferedFrameBuffer_->Init(context_.projection_->GetWindowSize());
	context_.shadowsFrameBuffer_->InitialiseFrameBuffer();

    for(auto& renderer : renderers)
    {
        renderer->Init();
    }
	postprocessingRenderersManager_.Init();
	Log("FullRenderer initialized.");
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
	postprocessingRenderersManager_.ReloadShaders();
}

template<class T>
void FullRenderer::AddRenderer()
{
	renderers.emplace_back(new T(context_));
}

void FullRenderer::CreateRenderers()
{
	AddRenderer<SkyBoxRenderer>();

	if (EngineConf.renderer.shadows.isEnabled)
		AddRenderer<ShadowMapRenderer>();

	if (EngineConf.renderer.flora.isGrass)
		AddRenderer<GrassRenderer>();

	AddRenderer<TerrainRenderer>();
	AddRenderer<TreeRenderer>();
	AddRenderer<PlantsRenderer>();
	AddRenderer<EntityRenderer>();

	if (EngineConf.renderer.particles.useParticles)
		AddRenderer<ParticlesRenderer>();

	AddRenderer<WaterRenderer>();
}
void FullRenderer::PostProcess(Scene * scene)
{
	postprocessingRenderersManager_.Render(scene);
}
void FullRenderer::Prepare(Scene *)
{
	context_.defferedFrameBuffer_->Clean();
}
} // GameEngine
