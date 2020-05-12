#include "BaseRenderer.h"

#include "Objects/Entity/EntityRenderer.h"
#include "Objects/Grass/GrassRenderer.h"
#include "Objects/Particles/ParticlesRenderer.h"
#include "Objects/Plants/PlantsRenderer.h"

#include "Objects/Shadows/ShadowMapRenderer.hpp"
#include "Objects/SkyBox/SkyBoxRenderer.h"
#include "Objects/Skydome/SkydomeRenderer.h"
#include "Objects/Terrain/Mesh/TerrainMeshRenderer.h"
#include "Objects/Terrain/TerrainRenderer.h"
#include "Objects/Tree/TreeRenderer.h"
#include "Objects/Water/WaterRenderer.h"

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"

#include "Logger/Log.h"

namespace GameEngine
{
BaseRenderer::BaseRenderer(RendererContext& context)
    : context_(context)
{
}
BaseRenderer::~BaseRenderer()
{
}

void BaseRenderer::Init()
{
    context_.graphicsApi_.SetShaderQuaility(GraphicsApi::ShaderQuaility::SimpleForwardRendering);
    CreateRenderers();
    InitRenderers();

    __RegisterRenderFunction__(RendererFunctionType::PRECONFIGURE, BaseRenderer::PreConfigure);
}
void BaseRenderer::Subscribe(GameObject* gameObject)
{
    for (auto& renderer : renderers)
        renderer->Subscribe(gameObject);
}
void BaseRenderer::UnSubscribe(GameObject* gameObject)
{
    for (auto& r : renderers)
        r->UnSubscribe(gameObject);
}
void BaseRenderer::UnSubscribeAll()
{
    for (auto& r : renderers)
        r->UnSubscribeAll();
}
void BaseRenderer::ReloadShaders()
{
    for (auto& renderer : renderers)
        renderer->ReloadShaders();
}
void BaseRenderer::InitRenderers()
{
    for (auto& renderer : renderers)
    {
        renderer->Init();
    }
}
void BaseRenderer::CreateRenderers()
{
    AddRenderer<SkyBoxRenderer>();
    AddRenderer<SkydomRenderer>();

    if (EngineConf.renderer.flora.isGrass)
        AddRenderer<GrassRenderer>();

    if (context_.graphicsApi_.IsTesselationSupported())
        AddRenderer<TerrainRenderer>();

    AddRenderer<TerrainMeshRenderer>();
    AddRenderer<TreeRenderer>();
    AddRenderer<PlantsRenderer>();
    AddRenderer<EntityRenderer>();

    if (EngineConf.renderer.particles.useParticles)
        AddRenderer<ParticlesRenderer>();

    AddRenderer<WaterRenderer>();
}
void BaseRenderer::PreConfigure(const Scene&, const Time&)
{
    context_.graphicsApi_.ClearBuffers({GraphicsApi::BufferType::COLOR, GraphicsApi::BufferType::DEPTH});
}
}  // namespace GameEngine
