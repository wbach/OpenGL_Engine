
#include "RenderersManager.h"

#include "DefferedRenderer.h"
#include "GUI/GuiRenderer.h"
#include "GUI/Text/GuiText.h"
#include "GUI/Texutre/GuiTexture.hpp"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Engine/AplicationContext.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerAppBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"

#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"

namespace GameEngine
{
struct RenderAsLine
{
    RenderAsLine(GraphicsApi::IGraphicsApi& graphicsApi, bool use)
        : graphicsApi_(graphicsApi)
        , use(use)
    {
        if (use)
            graphicsApi_.LineModeRender();
    }
    ~RenderAsLine()
    {
        if (use)
            graphicsApi_.PolygonModeRender();
    }
    GraphicsApi::IGraphicsApi& graphicsApi_;
    bool use = false;
};

namespace Renderer
{
RenderersManager::RenderersManager(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory)
    : graphicsApi_(graphicsApi)
    , renderAsLines(false)
    , markToReloadShaders_(false)
    , shaderFactory_(shaderFactory)
    , renderPhysicsDebug_(false)
{
}
const Projection& RenderersManager::GetProjection() const
{
    return projection_;
}
void RenderersManager::UpdateCamera(Scene* scene)
{
    scene->UpdateCamera();
}

void RenderersManager::Init()
{
    InitProjection();
    InitMainRenderer();
    InitGuiRenderer();
    CreateBuffers();

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

    graphicsApi_.EnableCulling();

    auto rendererType = EngineConf.renderer.type;

    auto registerFunc =
        std::bind(&RenderersManager::RegisterRenderFunction, this, std::placeholders::_1, std::placeholders::_2);

    if (rendererType == Params::RendererType::SIMPLE_RENDERER)
    {
        renderers_.emplace_back(new BaseRenderer(graphicsApi_, projection_, shaderFactory_, registerFunc));
    }
    else
    {
        renderers_.emplace_back(new DefferedRenderer(graphicsApi_, projection_, shaderFactory_, registerFunc));
    }
}
void RenderersManager::InitGuiRenderer()
{
    auto registerFunc =
        std::bind(&RenderersManager::RegisterRenderFunction, this, std::placeholders::_1, std::placeholders::_2);
    guiContext_.renderer = new GUIRenderer(registerFunc);
    guiContext_.texts    = new GameEngine::GuiText(graphicsApi_, "GUI/consola.ttf", shaderFactory_);
    guiContext_.texures  = new Renderer::Gui::GuiTexture(graphicsApi_, shaderFactory_);
    guiContext_.renderer->AddElement(guiContext_.texures);
    guiContext_.renderer->AddElement(guiContext_.texts);
    renderers_.emplace_back(guiContext_.renderer);
}
void RenderersManager::RegisterRenderFunction(RendererFunctionType type, RendererFunction function)
{
    rendererFunctions_[type].push_back(function);
}
void RenderersManager::RenderScene(Scene* scene, const Time& threadTime)
{
    if (scene == nullptr)
        return;

    ReloadShadersExecution();
    bufferDataUpdater_.Update();
    UpdateCamera(scene);
    UpdatePerFrameBuffer(scene);

    RenderAsLine lineMode(graphicsApi_, renderAsLines.load());

    Render(RendererFunctionType::PRERENDER, scene, threadTime);
    Render(RendererFunctionType::PRECONFIGURE, scene, threadTime);
    Render(RendererFunctionType::CONFIGURE, scene, threadTime);
    Render(RendererFunctionType::UPDATE, scene, threadTime);
    Render(RendererFunctionType::POSTUPDATE, scene, threadTime);
    Render(RendererFunctionType::ONENDFRAME, scene, threadTime);
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

    bufferDataUpdater_.Subscribe(gameObject);

    for (auto& renderer : renderers_)
        renderer->Subscribe(gameObject);
}
void RenderersManager::UnSubscribe(GameObject* gameObject)
{
    bufferDataUpdater_.UnSubscribe(gameObject);

    for (auto& r : renderers_)
        r->UnSubscribe(gameObject);
}
void RenderersManager::UnSubscribeAll()
{
    for (auto& r : renderers_)
        r->UnSubscribeAll();

    bufferDataUpdater_.UnSubscribeAll();
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
void RenderersManager::SetPhysicsDebugDraw(std::function<void(const mat4&, const mat4&)> func_)
{
    physicsDebugDraw_ = func_;
}
void RenderersManager::EnableDrawPhysicsDebyg()
{
    renderPhysicsDebug_ = true;
}
void RenderersManager::DisableDrawPhysicsDebyg()
{
    renderPhysicsDebug_ = false;
}
void RenderersManager::Render(RendererFunctionType type, Scene* scene, const Time& threadTime)
{
    if (scene == nullptr)
        return;

    if (rendererFunctions_.count(type))
    {
        for (auto& f : rendererFunctions_.at(type))
            f(*scene, threadTime);
    }

    if (renderPhysicsDebug_ and physicsDebugDraw_)
        physicsDebugDraw_(scene->GetCamera().GetViewMatrix(), projection_.GetProjectionMatrix());
}
void RenderersManager::CreateBuffers()
{
    CreatePerAppBuffer();
    CreatePerFrameBuffer();
}
void RenderersManager::CreatePerAppBuffer()
{
    auto perAppId = graphicsApi_.CreateShaderBuffer(PER_APP_BIND_LOCATION, sizeof(PerAppBuffer));

    if (perAppId)
    {
        PerAppBuffer perApp;
        perApp.clipPlane       = vec4{0.f, 1.f, 0.f, 100000.f};
        perApp.shadowVariables = vec3(0.f, 10.f, 10.f);
        perApp.useTextures     = 1.f;
        perApp.viewDistance    = 500.f;
        graphicsApi_.UpdateShaderBuffer(*perAppId, &perApp);
        graphicsApi_.BindShaderBuffer(*perAppId);
    }
}

void RenderersManager::CreatePerFrameBuffer()
{
    if (not perFrameId_)
    {
        perFrameId_ = graphicsApi_.CreateShaderBuffer(PER_FRAME_BIND_LOCATION, sizeof(PerFrameBuffer));
    }
    if (perFrameId_)
    {
        PerFrameBuffer buffer;
        buffer.ProjectionViewMatrix =
            projection_.GetProjectionMatrix() * glm::lookAt(glm::vec3(0, 0, -5), glm::vec3(0), glm::vec3(0, 1, 0));
        buffer.ToShadowMapSpace = mat4();
        buffer.cameraPosition   = vec3(0);
        graphicsApi_.UpdateShaderBuffer(*perFrameId_, &buffer);
        graphicsApi_.BindShaderBuffer(*perFrameId_);
    }
}

void RenderersManager::UpdatePerFrameBuffer(Scene* scene)
{
    if (perFrameId_)
    {
        PerFrameBuffer buffer;
        buffer.ProjectionViewMatrix = projection_.GetProjectionMatrix() * scene->GetCamera().GetViewMatrix();
        buffer.cameraPosition       = scene->GetCamera().GetPosition();
        graphicsApi_.UpdateShaderBuffer(*perFrameId_, &buffer);
    }
}

}  // namespace Renderer
}  // namespace GameEngine
