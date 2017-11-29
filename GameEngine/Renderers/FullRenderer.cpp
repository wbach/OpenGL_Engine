#include "FullRenderer.h"
#include "LightPassRenderer.h"

#include "Grass/GrassRenderer.h"
#include "Entity/EntityRenderer.h"
#include "SkyBox/SkyBoxRenderer.h"
#include "Shadows/ShadowMapRenderer.hpp"
#include "Shadows/ShadowFrameBuffer.h"
#include "Terrain/Tesselation/TessellationTerrainRenderer.h"
#include "Framebuffer/DeferedFrameBuffer/DeferedFrameBuffer.h"

#include "../Engine/Configuration.h"
#include "../Engine/Projection.h"
#include "Logger/Log.h"

FullRenderer::FullRenderer(CProjection* projection_matrix)
	: projectionMatrix(projection_matrix)
	, defferedFrameBuffer(new CDefferedFrameBuffer())
    , shadowsFrameBuffer(new CShadowFrameBuffer())
{	
    if(EngineConf.isShadows)
        renderers.emplace_back(new CShadowMapRenderer(projection_matrix, shadowsFrameBuffer.get()));

    if(EngineConf.advancedGrass)
		renderers.emplace_back(new CGrassRenderer(projection_matrix, defferedFrameBuffer.get()));

	renderers.emplace_back(new CSkyBoxRenderer(projection_matrix, defferedFrameBuffer.get()));
    renderers.emplace_back(new CTessellationTerrainRenderer(projection_matrix, defferedFrameBuffer.get(), shadowsFrameBuffer.get()));
	renderers.emplace_back(new CEntityRenderer(projection_matrix, defferedFrameBuffer.get()));
	renderers.emplace_back(new CLightPassRenderer(projection_matrix, defferedFrameBuffer.get()));
}

void FullRenderer::Init()
{
	defferedFrameBuffer->Init(projectionMatrix->GetWindowSize());

     for(auto& renderer : renderers)
    {
        renderer->Init();
    }
	Log("FullRenderer initialized.");
}
void FullRenderer::PrepareFrame(CScene* scene)
{
	defferedFrameBuffer->Clean();

    for(auto& renderer : renderers)
    {
        renderer->PrepareFrame(scene);
    }
}
void FullRenderer::Render(CScene* scene)
{
    for(auto& renderer : renderers)
    {
        renderer->Render(scene);
    }
}
void FullRenderer::EndFrame(CScene* scene)
{
    for(auto& renderer : renderers)
    {
        renderer->EndFrame(scene);
    }
}

void FullRenderer::Subscribe(CGameObject * gameObject)
{
	for (auto& renderer : renderers)
		renderer->Subscribe(gameObject);
}

void FullRenderer::ReloadShaders()
{
	for (auto& renderer : renderers)
	{
		renderer->ReloadShaders();
	}
}
