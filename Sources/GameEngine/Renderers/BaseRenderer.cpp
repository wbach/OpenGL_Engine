
#include "BaseRenderer.h"

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "Logger/Log.h"
#include "Objects/Entity/ConcreteEntityRenderer.h"
#include "Objects/Grass/GrassRenderer.h"
#include "Objects/Particles/ParticlesRenderer.h"
#include "Objects/Shadows/ShadowMapRenderer.hpp"
#include "Objects/SkyBox/SkyBoxRenderer.h"
#include "Objects/Skydome/SkydomeRenderer.h"
#include "Objects/Terrain/Mesh/ConcreteTerrainMeshRenderer.h"
#include "Objects/Terrain/TerrainRenderer.h"
#include "Objects/Tree/TreeRenderer.h"
#include "Objects/Water/WaterRenderer.h"

namespace GameEngine
{
BaseRenderer::BaseRenderer(RendererContext& context)
    : context_(context)
{
}
BaseRenderer::~BaseRenderer()
{
}
void BaseRenderer::init()
{
    context_.graphicsApi_.SetShaderQuaility(GraphicsApi::ShaderQuaility::SimpleForwardRendering);
    createRenderers();
    initRenderers();
}
void BaseRenderer::prepare()
{
    for (auto& renderer : renderers)
        renderer->prepare();
}
void BaseRenderer::subscribe(GameObject& gameObject)
{
    for (auto& renderer : renderers)
        renderer->subscribe(gameObject);
}
void BaseRenderer::unSubscribe(GameObject& gameObject)
{
    for (auto& r : renderers)
        r->unSubscribe(gameObject);
}
void BaseRenderer::unSubscribeAll()
{
    for (auto& r : renderers)
        r->unSubscribeAll();
}
void BaseRenderer::reloadShaders()
{
    for (auto& renderer : renderers)
        renderer->reloadShaders();
}
void BaseRenderer::initRenderers()
{
    for (auto& renderer : renderers)
        renderer->init();
}
void BaseRenderer::render()
{
    for (auto& renderer : renderers)
        renderer->render();
}
void BaseRenderer::blendRender()
{
    for (auto& renderer : renderers)
        renderer->blendRender();
}
void BaseRenderer::createRenderers()
{
    addRenderer<SkyBoxRenderer>();
    addRenderer<SkydomRenderer>();
    if (context_.graphicsApi_.IsTesselationSupported())
        addRenderer<TerrainRenderer>();
    addRenderer<ConcreteTerrainMeshRenderer>();
    addRenderer<TreeRenderer>();
    if (EngineConf.renderer.flora.isGrass)
        addRenderer<GrassRenderer>();
    addRenderer<ConcreteEntityRenderer>();
    if (EngineConf.renderer.particles.useParticles)
        addRenderer<ParticlesRenderer>();
    addRenderer<WaterRenderer>();
}
}  // namespace GameEngine
