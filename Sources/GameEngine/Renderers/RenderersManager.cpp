
#include "RenderersManager.h"

#include "DefferedRenderer.h"
#include "GUI/GuiRenderer.h"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/ShaderBuffers/PerAppBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"
#include "RendererContext.h"

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
RenderersManager::RenderersManager(GraphicsApi::IGraphicsApi& graphicsApi, Utils::MeasurementHandler& measurmentHandler,
                                   Utils::Thread::ThreadSync& threadSync)
    : graphicsApi_(graphicsApi)
    , measurmentHandler_(measurmentHandler)
    , renderAsLines(false)
    , markToReloadShaders_(false)
    , guiRenderer_(graphicsApi)
    , debugRenderer_(graphicsApi, threadSync)
    , viewProjectionMatrix_(1.f)
    , bufferDataUpdater_(graphicsApi)
{
}
RenderersManager::~RenderersManager()
{
    DEBUG_LOG("destructor");

    if (perFrameId_)
    {
        graphicsApi_.DeleteShaderBuffer(*perFrameId_);
    }
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
    debugRenderer_.Init();
    CreateBuffers();

    for (auto& r : renderers_)
        r->Init();
}
void RenderersManager::InitProjection()
{
    projection_.Init(EngineConf.renderer.resolution);
    projection_.CreateProjectionMatrix();
}
void RenderersManager::InitMainRenderer()
{
    if (!renderers_.empty())
        return;

    graphicsApi_.EnableCulling();

    auto rendererType = EngineConf.renderer.type;

    auto registerFunc =
        std::bind(&RenderersManager::RegisterRenderFunction, this, std::placeholders::_1, std::placeholders::_2);

    rendererContext_ =
        std::make_unique<RendererContext>(projection_, frustrum_, graphicsApi_, measurmentHandler_, registerFunc);

    auto supportedRenderers = graphicsApi_.GetSupportedRenderers();

    if (supportedRenderers.empty())
    {
        ERROR_LOG("Graphics api not supporting any renderer!");
        return;
    }

    if (rendererType == GraphicsApi::RendererType::SIMPLE)
    {
        auto iter = std::find(supportedRenderers.begin(), supportedRenderers.end(), GraphicsApi::RendererType::SIMPLE);
        if (iter != supportedRenderers.end())
        {
            DEBUG_LOG("Create base renderer");
            renderers_.emplace_back(new BaseRenderer(*rendererContext_));
        }
        else
        {
            DEBUG_LOG("Graphics api are not supporting SIMPLE renderer try using full");
            DEBUG_LOG("Create deffered renderer");
            renderers_.emplace_back(new DefferedRenderer(*rendererContext_));
        }
        return;
    }

    if (rendererType == GraphicsApi::RendererType::FULL)
    {
        auto iter = std::find(supportedRenderers.begin(), supportedRenderers.end(), GraphicsApi::RendererType::FULL);
        if (iter != supportedRenderers.end())
        {
            DEBUG_LOG("Create deffered renderer");
            renderers_.emplace_back(new DefferedRenderer(*rendererContext_));
        }
        else
        {
            DEBUG_LOG("Graphics api are not supporting FULL renderer try using simple");
            DEBUG_LOG("Create base renderer");
            renderers_.emplace_back(new BaseRenderer(*rendererContext_));
        }
        return;
    }
}
void RenderersManager::InitGuiRenderer()
{
    guiRenderer_.Init();
}
void RenderersManager::RegisterRenderFunction(RendererFunctionType type, RendererFunction function)
{
    rendererFunctions_[type].push_back(function);
}
void RenderersManager::RenderScene(Scene* scene, const Time& threadTime)
{
    graphicsApi_.PrepareFrame();

    if (scene == nullptr)
        return;
    ReloadShadersExecution();
    bufferDataUpdater_.Update();
    UpdateCamera(scene);

    viewProjectionMatrix_ = projection_.GetProjectionMatrix() * scene->GetCamera().GetViewMatrix();
    frustrum_.CalculatePlanes(viewProjectionMatrix_);
    UpdatePerFrameBuffer(scene);

    {
        RenderAsLine lineMode(graphicsApi_, renderAsLines.load());

        Render(RendererFunctionType::PRERENDER, scene, threadTime);
        Render(RendererFunctionType::PRECONFIGURE, scene, threadTime);
        Render(RendererFunctionType::CONFIGURE, scene, threadTime);
        Render(RendererFunctionType::UPDATE, scene, threadTime);
        Render(RendererFunctionType::POSTUPDATE, scene, threadTime);
        Render(RendererFunctionType::ONENDFRAME, scene, threadTime);
    }

    debugRenderer_.Render(*scene, threadTime);
    guiRenderer_.Render(*scene, threadTime);

    if (unsubscribeAllCallback_)
    {
        UnSubscribeAll();
        unsubscribeAllCallback_();
        unsubscribeAllCallback_ = {};
    }
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

    guiRenderer_.ReloadShaders();

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
    guiRenderer_.UnSubscribeAll();
}
void RenderersManager::UnSubscribeAll(std::function<void()> callback)
{
    unsubscribeAllCallback_ = callback;
}
void RenderersManager::SwapLineFaceRender()
{
    renderAsLines.store(!renderAsLines.load());
}

void RenderersManager::setLineRenderMode(bool v)
{
    renderAsLines.store(v);
}

bool RenderersManager::getLineRenderMode() const
{
    return renderAsLines.load();
}

GUIRenderer& RenderersManager::GetGuiRenderer()
{
    return guiRenderer_;
}

DebugRenderer& RenderersManager::GetDebugRenderer()
{
    return debugRenderer_;
}

bool RenderersManager::IsTesselationSupported() const
{
    return graphicsApi_.IsTesselationSupported();
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
}
void RenderersManager::CreateBuffers()
{
    CreatePerAppBuffer();
    CreatePerFrameBuffer();
}
void RenderersManager::CreatePerAppBuffer()
{
    if (not perAppId_)
        perAppId_ = graphicsApi_.CreateShaderBuffer(PER_APP_BIND_LOCATION, sizeof(PerAppBuffer));

    UpdatePerAppBuffer();
}
namespace
{
float F(bool v)
{
    return v ? 1.f : 0.f;
}
}  // namespace
void RenderersManager::UpdatePerAppBuffer() const
{
    if (perAppId_)
    {
        const auto& textureConfig  = EngineConf.renderer.textures;
        const auto& shadowsConfig  = EngineConf.renderer.shadows;
        const auto& floraConfig    = EngineConf.renderer.flora;
        const auto& rendererConfig = EngineConf.renderer;

        PerAppBuffer perApp;
        perApp.clipPlane   = vec4{0.f, 1.f, 0.f, 100000.f};
        perApp.useTextures = vec4(F(textureConfig.useDiffuse), F(textureConfig.useNormal), F(textureConfig.useSpecular),
                                  F(textureConfig.useDisplacement));
        perApp.shadowVariables = vec4(F(shadowsConfig.isEnabled), shadowsConfig.distance, shadowsConfig.mapSize, 0.f);
        perApp.viewDistance    = vec4(rendererConfig.viewDistance, rendererConfig.normalMappingDistance,
                                   floraConfig.viewDistance, rendererConfig.viewDistance);
        graphicsApi_.UpdateShaderBuffer(*perAppId_, &perApp);
        graphicsApi_.BindShaderBuffer(*perAppId_);
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
        buffer.ToShadowMapSpace = mat4(1.f);
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
        buffer.ProjectionViewMatrix = graphicsApi_.PrepareMatrixToLoad(viewProjectionMatrix_);
        buffer.cameraPosition       = scene->GetCamera().GetPosition();
        graphicsApi_.UpdateShaderBuffer(*perFrameId_, &buffer);
    }
}
}  // namespace Renderer
}  // namespace GameEngine
