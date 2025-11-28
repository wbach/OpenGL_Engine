
#include "BaseRenderer.h"

#include <Logger/Log.h>

#include "GameEngine/Engine/Configuration.h"
#include "Objects/Entity/ConcreteEntityRenderer.h"
#include "Objects/Entity/PreviewRenderer.h"
#include "Objects/SkyBox/ConcreteSkyBoxRenderer.h"
#include "Objects/Terrain/Mesh/ConcreteTerrainMeshRenderer.h"
#include "Objects/Tree/TreeRenderer.h"
#include "Objects/Water/WaterRenderer.h"
#include "Time/Timer.h"

namespace GameEngine
{
BaseRenderer::BaseRenderer(RendererContext& context)
    : context_(context)
    , renderTarget(nullptr)
{
}
BaseRenderer::~BaseRenderer()
{
}
void BaseRenderer::init()
{
    context_.graphicsApi_.SetShaderQuaility(GraphicsApi::ShaderQuaility::SimpleForwardRendering);
    createBaseRenderers();
    initRenderers();
}
void BaseRenderer::prepare()
{
    for (auto& renderer : renderers)
    {
        Utils::Timer timer;
        renderer.ptr->prepare();
        if (renderer.prepareTime)
            renderer.prepareTime->value = timer.FormatElapsed();
    }
}
void BaseRenderer::subscribe(GameObject& gameObject)
{
    for (auto& renderer : renderers)
        renderer.ptr->subscribe(gameObject);
}
void BaseRenderer::unSubscribe(GameObject& gameObject)
{
    for (auto& renderer : renderers)
        renderer.ptr->unSubscribe(gameObject);
}
void BaseRenderer::unSubscribe(const Components::IComponent& component)
{
    for (auto& renderer : renderers)
        renderer.ptr->unSubscribe(component);
}
void BaseRenderer::unSubscribeAll()
{
    for (auto& renderer : renderers)
        renderer.ptr->unSubscribeAll();
}
void BaseRenderer::reloadShaders()
{
    context_.graphicsApi_.SetShaderQuaility(GraphicsApi::ShaderQuaility::SimpleForwardRendering);
    for (auto& renderer : renderers)
        renderer.ptr->reloadShaders();
}
void BaseRenderer::initRenderers()
{
    for (auto& renderer : renderers)
        renderer.ptr->init();
}
void BaseRenderer::render()
{
    if (context_.camera_)
    {
        bindTarget();
        renderImpl();
    }
    else
    {
        LOG_ERROR << "Camera not set!";
    }
}
void BaseRenderer::setViewPort()
{
    context_.graphicsApi_.SetViewPort(0, 0, EngineConf.window.size->x, EngineConf.window.size->y);
}
void BaseRenderer::blendRender()
{
    for (auto& renderer : renderers)
        renderer.ptr->blendRender();
}
void BaseRenderer::createBaseRenderers()
{
    //addRenderer<PreviewRenderer>();
    addRenderer<ConcreteSkyBoxRenderer>();
    addRenderer<ConcreteTerrainMeshRenderer>();
    addRenderer<TreeRenderer>();
    addRenderer<ConcreteEntityRenderer>();
    addRenderer<WaterRenderer>();
}
void BaseRenderer::renderImpl()
{
    for (auto& renderer : renderers)
    {
        Utils::Timer timer;
        renderer.ptr->render();
        if (renderer.renderTime)
            renderer.renderTime->value = timer.FormatElapsed();
    }
}
void BaseRenderer::setRenderTarget(GraphicsApi::IFrameBuffer* target)
{
    renderTarget = target;
}
void BaseRenderer::bindTarget()
{
    if (renderTarget)
    {
        renderTarget->Clear();
        context_.graphicsApi_.SetViewPort(0, 0, renderTarget->GetSize().x, renderTarget->GetSize().y);
        renderTarget->Bind(GraphicsApi::FrameBuffer::BindType::Write);
    }
    else
    {
        setViewPort();
        context_.graphicsApi_.GetDefaultFrameBuffer().Bind();
    }
}
void BaseRenderer::cleanUp()
{
    for (auto& renderer : renderers)
        renderer.ptr->cleanUp();
}
}  // namespace GameEngine
