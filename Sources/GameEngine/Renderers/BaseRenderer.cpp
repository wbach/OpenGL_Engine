
#include "BaseRenderer.h"

#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Projection.h"
#include "Logger/Log.h"
#include "Objects/Entity/ConcreteEntityRenderer.h"
#include "Objects/Entity/PreviewRenderer.h"
#include "Objects/Grass/GrassRenderer.h"
#include "Objects/Particles/ParticlesRenderer.h"
#include "Objects/Shadows/ShadowMapRenderer.hpp"
#include "Objects/SkyBox/ConcreteSkyBoxRenderer.h"
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
    subscribeJointPoseUpdater(gameObject);

    for (auto& renderer : renderers)
        renderer->subscribe(gameObject);
}
void BaseRenderer::unSubscribe(GameObject& gameObject)
{
    subscribers_.erase(gameObject.GetId());

    for (auto& r : renderers)
        r->unSubscribe(gameObject);
}
void BaseRenderer::unSubscribeAll()
{
    subscribers_.clear();

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
    setViewPort();

    for (auto& [_, posUpdater] : subscribers_)
    {
        posUpdater->updateGameObjectTransform();
    }

    for (auto& renderer : renderers)
        renderer->render();
}
void BaseRenderer::setViewPort()
{
    context_.graphicsApi_.SetViewPort(0, 0, EngineConf.window.size->x, EngineConf.window.size->y);
}
void BaseRenderer::blendRender()
{
    for (auto& renderer : renderers)
        renderer->blendRender();
}
void BaseRenderer::createRenderers()
{
    addRenderer<PreviewRenderer>();
    addRenderer<ConcreteSkyBoxRenderer>();
    addRenderer<SkydomRenderer>();
    addRenderer<TerrainRenderer>();
    addRenderer<ConcreteTerrainMeshRenderer>();
    addRenderer<TreeRenderer>();
    addRenderer<GrassRenderer>();
    addRenderer<ConcreteEntityRenderer>();
    addRenderer<ParticlesRenderer>();
    addRenderer<WaterRenderer>();
    addRenderer<ShadowMapRenderer>();
}
void BaseRenderer::subscribeJointPoseUpdater(GameObject& gameobject)
{
    auto iter = subscribers_.find(gameobject.GetId());

    if (iter != subscribers_.end())
        return;

    auto jointPoseUpdater = gameobject.GetComponent<Components::JointPoseUpdater>();

    if (jointPoseUpdater == nullptr)
        return;

    subscribers_.insert({gameobject.GetId(), jointPoseUpdater});
}
}  // namespace GameEngine
