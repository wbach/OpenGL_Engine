#include "RenderersManager.h"

#include "FullRenderer.h"
#include "GUI/GuiRenderer.h"
#include "GUI/Text/GuiText.h"
#include "GUI/Texutre/GuiTexture.hpp"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Engine/AplicationContext.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
#include "SimpleRenderer.h"

namespace GameEngine
{
struct RenderAsLine
{
    RenderAsLine(const IGraphicsApiPtr& graphicsApi, bool use)
        : graphicsApi_(graphicsApi)
        , use(use)
    {
        if (use)
            graphicsApi_->LineModeRender();
    }
    ~RenderAsLine()
    {
        if (use)
            graphicsApi_->PolygonModeRender();
    }
    const IGraphicsApiPtr& graphicsApi_;
    bool use = false;
};

namespace Renderer
{
RenderersManager::RenderersManager(IGraphicsApiPtr graphicsApi)
    : graphicsApi_(graphicsApi)
    , renderAsLines(false)
    , markToReloadShaders_(false)
{
}
const Projection& RenderersManager::GetProjection() const
{
    return projection_;
}
void RenderersManager::UpdateCamera(Scene* scene)
{
    auto camera = scene->GetCamera();

    if (camera == nullptr)
        return;

    camera->CalculateInput();
    camera->Move();
    camera->UpdateMatrix();
}
void RenderersManager::TakeSnapShoots()
{
    for (auto& obj : dynamincObjects_)
        obj->worldTransform.TakeSnapShoot();
}
void RenderersManager::Init()
{
    InitProjection();
    InitMainRenderer();
    InitGuiRenderer();

    for (auto& r : renderers_)
        r->Init();
}
void RenderersManager::InitProjection()
{
    auto& conf  = EngineConf;
    projection_ = conf.renderer.resolution;
}
void RenderersManager::InitMainRenderer()
{
    if (!renderers_.empty())
        return;

    auto rendererType = EngineConf.renderer.type;

    auto registerFunc =
        std::bind(&RenderersManager::RegisterRenderFunction, this, std::placeholders::_1, std::placeholders::_2);

    if (rendererType == Params::RendererType::FULL)
        renderers_.emplace_back(new FullRenderer(graphicsApi_, &projection_, registerFunc));
    else
        renderers_.emplace_back(new SimpleRenderer(graphicsApi_, &projection_, registerFunc));
}
void RenderersManager::InitGuiRenderer()
{
    auto registerFunc =
        std::bind(&RenderersManager::RegisterRenderFunction, this, std::placeholders::_1, std::placeholders::_2);
    guiContext_.renderer = new GUIRenderer(registerFunc);
    guiContext_.texts    = new GameEngine::GuiText(graphicsApi_, "GUI/consola.ttf");
    guiContext_.texures  = new Renderer::Gui::GuiTexture(graphicsApi_);
    guiContext_.renderer->AddElement(guiContext_.texures);
    guiContext_.renderer->AddElement(guiContext_.texts);
    renderers_.emplace_back(guiContext_.renderer);
}
void RenderersManager::RegisterRenderFunction(RendererFunctionType type, RendererFunction function)
{
    rendererFunctions_[type].push_back(function);
}
void RenderersManager::RenderScene(Scene* scene)
{
    if (scene == nullptr)
        return;

    TakeSnapShoots();
    UpdateCamera(scene);
    ReloadShadersExecution();

    RenderAsLine lineMode(graphicsApi_, renderAsLines.load());

    Render(RendererFunctionType::PRECONFIGURE, scene);
    Render(RendererFunctionType::CONFIGURE, scene);
    Render(RendererFunctionType::UPDATE, scene);
    Render(RendererFunctionType::POSTUPDATE, scene);
    Render(RendererFunctionType::ONENDFRAME, scene);
}
void RenderersManager::ReloadShaders()
{
    markToReloadShaders_.store(true);
}
void RenderersManager::ReloadShadersExecution()
{
    if (!markToReloadShaders_.load())
        return;

    for (auto& renderer : renderers_)
        renderer->ReloadShaders();

    markToReloadShaders_.store(false);
}
void RenderersManager::Subscribe(GameObject* gameObject)
{
    if (gameObject == nullptr)
        return;

    if (gameObject->worldTransform.isDynamic_)
        dynamincObjects_.push_back(gameObject);

    for (auto& renderer : renderers_)
        renderer->Subscribe(gameObject);
}
void RenderersManager::UnSubscribe(GameObject* gameObject)
{
    for (auto iter = dynamincObjects_.begin(); iter != dynamincObjects_.end();)
    {
        if (gameObject->GetId())
        {
            iter = dynamincObjects_.erase(iter);
            break;
        }
        else
        {
            ++iter;
        }
    }

    for (auto& r : renderers_)
        r->UnSubscribe(gameObject);
}
void RenderersManager::UnSubscribeAll()
{
    for (auto& r : renderers_)
        r->UnSubscribeAll();

    dynamincObjects_.clear();
}
void RenderersManager::SwapLineFaceRender()
{
    renderAsLines.store(!renderAsLines.load());
}
GuiTextElement& RenderersManager::GuiText(const std::string& label)
{
    return guiContext_.texts->texts[label];
}
Gui::GuiTextureElement& RenderersManager::GuiTexture(const std::string& label)
{
    return guiContext_.texures->guiTextures_[label];
}
void RenderersManager::Render(RendererFunctionType type, Scene* scene)
{
    for (auto& f : rendererFunctions_[type])
        f(scene);
}

}  // Renderer
}  // GameEngine
