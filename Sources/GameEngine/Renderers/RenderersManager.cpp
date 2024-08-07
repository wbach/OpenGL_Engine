
#include "RenderersManager.h"

#include "DefferedRenderer.h"
#include "GUI/GuiRenderer.h"
#include "GameEngine/Camera/Camera.h"
#include "GameEngine/Components/Renderer/Entity/RendererComponent.hpp"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"
#include "Logger/Log.h"

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
namespace
{
float F(bool v)
{
    return v ? 1.f : 0.f;
}
}  // namespace

namespace Renderer
{
RenderersManager::RenderersManager(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuLoader,
                                   Utils::MeasurementHandler& measurmentHandler, Utils::Thread::ThreadSync& threadSync,
                                   const Time& renderThreadTime)
    : graphicsApi_(graphicsApi)
    , gpuLoader_(gpuLoader)
    , measurmentHandler_(measurmentHandler)
    , renderAsLines(false)
    , markToReloadShaders_(false)
    , guiRenderer_(graphicsApi)
    , viewProjectionMatrix_(1.f)
    , bufferDataUpdater_()
    , rendererContext_(projection_, frustrum_, graphicsApi_, gpuLoader_, measurmentHandler_, renderThreadTime)
    , debugRenderer_(rendererContext_, threadSync)
{
    frustrumCheckCount_ = &measurmentHandler_.AddNewMeasurment("FrustrumCheckCount", "0");

    shadowEnabledSubscriptionId_ = EngineConf.renderer.shadows.isEnabled.subscribeForChange(
        [this]() { gpuLoader_.AddFunctionToCall([this]() { UpdatePerAppBuffer(); }); });

    viewDistanceSubscriptionId_ = EngineConf.renderer.viewDistance.subscribeForChange(
        [this]() { gpuLoader_.AddFunctionToCall([this]() { UpdatePerAppBuffer(); }); });
}
RenderersManager::~RenderersManager()
{
    DEBUG_LOG("destructor");

    EngineConf.renderer.shadows.isEnabled.unsubscribe(shadowEnabledSubscriptionId_);
    EngineConf.renderer.viewDistance.unsubscribe(viewDistanceSubscriptionId_);

    if (perFrameId_)
    {
        graphicsApi_.DeleteShaderBuffer(*perFrameId_);
    }
}
const Projection& RenderersManager::GetProjection() const
{
    return projection_;
}
void RenderersManager::Init()
{
    InitProjection();
    InitGuiRenderer();
    debugRenderer_.init();
    CreateBuffers();

    createMainRenderer();
    mainRenderer_->init();
}
void RenderersManager::InitProjection()
{
    projection_.Init();
    projection_.CreateProjectionMatrix();
}
void RenderersManager::createMainRenderer()
{
    graphicsApi_.EnableCulling();

    const auto rendererType = EngineConf.renderer.type.get();
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
            mainRenderer_ = std::make_unique<BaseRenderer>(rendererContext_);
        }
        else
        {
            DEBUG_LOG("Graphics api are not supporting SIMPLE renderer try using full");
            DEBUG_LOG("Create deffered renderer");
            mainRenderer_ = std::make_unique<DefferedRenderer>(rendererContext_);
        }
        return;
    }

    if (rendererType == GraphicsApi::RendererType::FULL)
    {
        auto iter = std::find(supportedRenderers.begin(), supportedRenderers.end(), GraphicsApi::RendererType::FULL);
        if (iter != supportedRenderers.end())
        {
            DEBUG_LOG("Create deffered renderer");
            mainRenderer_ = std::make_unique<DefferedRenderer>(rendererContext_);
        }
        else
        {
            DEBUG_LOG("Graphics api are not supporting FULL renderer try using simple");
            DEBUG_LOG("Create base renderer");
            mainRenderer_ = std::make_unique<BaseRenderer>(rendererContext_);
        }
        return;
    }
}
void RenderersManager::InitGuiRenderer()
{
    guiRenderer_.Init();
}
void RenderersManager::renderScene(Scene& scene)
{
    rendererContext_.scene_ = &scene;

    ReloadShadersExecution();
    bufferDataUpdater_.Update();
    scene.UpdateCamera();

    viewProjectionMatrix_ = projection_.GetProjectionMatrix() * scene.GetCamera().GetViewMatrix();
    frustrum_.prepareFrame(viewProjectionMatrix_);
    updatePerFrameBuffer(scene);

    mainRenderer_->prepare();
    {
        RenderAsLine lineMode(graphicsApi_, renderAsLines.load());
        graphicsApi_.EnableDepthTest();
        mainRenderer_->render();
        mainRenderer_->blendRender();
    }

    *frustrumCheckCount_ = std::to_string(frustrum_.getIntersectionsCountInFrame());

    debugRenderer_.render();
    guiRenderer_.render();

    /*debugRenderer_.renderTextures(
        {rendererContext_.cascadedShadowMapsIds_[0], rendererContext_.cascadedShadowMapsIds_[1],
         rendererContext_.cascadedShadowMapsIds_[2], rendererContext_.cascadedShadowMapsIds_[3]});*/

    // debugRenderer_.renderTextures({rendererContext_.waterReflectionTextureId_,
    //                               rendererContext_.waterRefractionTextureId_,
    //                               rendererContext_.waterRefractionDepthTextureId_});

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
    if (not markToReloadShaders_.load())
        return;

    mainRenderer_->reloadShaders();
    guiRenderer_.ReloadShaders();
    debugRenderer_.reloadShaders();

    markToReloadShaders_.store(false);
}
void RenderersManager::Subscribe(GameObject* gameObject)
{
    if (not gameObject)
        return;

    debugRenderer_.subscribe(*gameObject);
    bufferDataUpdater_.Subscribe(gameObject);
    mainRenderer_->subscribe(*gameObject);
}
void RenderersManager::UnSubscribe(GameObject* gameObject)
{
    if (gameObject)
    {
        debugRenderer_.unSubscribe(*gameObject);
        bufferDataUpdater_.UnSubscribe(gameObject);
        mainRenderer_->unSubscribe(*gameObject);
    }
}

void RenderersManager::UnSubscribeAll()
{
    debugRenderer_.unSubscribeAll();
    mainRenderer_->unSubscribeAll();
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

void RenderersManager::UpdatePerAppBuffer()
{
    if (perAppId_)
    {
        const auto& textureConfig  = EngineConf.renderer.textures;
        auto& shadowsConfig        = EngineConf.renderer.shadows;
        const auto& floraConfig    = EngineConf.renderer.flora;
        const auto& rendererConfig = EngineConf.renderer;

        perApp_.useTextures = vec4(F(textureConfig.useDiffuse), F(textureConfig.useNormal),
                                   F(textureConfig.useSpecular), F(textureConfig.useDisplacement));
        perApp_.shadowVariables =
            vec4(F(*shadowsConfig.isEnabled), *shadowsConfig.distance, *shadowsConfig.mapSize, 0.f);
        perApp_.viewDistance = vec4(*rendererConfig.viewDistance, *rendererConfig.normalMappingDistance,
                                    *floraConfig.viewDistance, *rendererConfig.viewDistance);

        perApp_.fogData = vec4(rendererContext_.fogColor_, 3.5f);
        graphicsApi_.UpdateShaderBuffer(*perAppId_, &perApp_);
        graphicsApi_.BindShaderBuffer(*perAppId_);
    }
}
vec2 RenderersManager::convertToNdcPosition(const vec3& point) const
{
    auto clipSpace = viewProjectionMatrix_ * vec4(point, 1.f);
    return vec2(clipSpace.x, clipSpace.y) / clipSpace.w;
}
vec3 RenderersManager::convertToNdcPosition2(const vec3& point) const
{
    auto clipSpace = viewProjectionMatrix_ * vec4(point, 1.f);
    return vec3(clipSpace.x, clipSpace.y, clipSpace.z) / clipSpace.w;
}
vec2 RenderersManager::convertToScreenPosition(const vec3& point) const
{
    return convertToNdcPosition(point) / 2.f + 0.5f;
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
        buffer.cameraPosition = vec3(0);
        buffer.clipPlane      = vec4{0.f, 1.f, 0.f, 100000.f};
        graphicsApi_.UpdateShaderBuffer(*perFrameId_, &buffer);
        graphicsApi_.BindShaderBuffer(*perFrameId_);
    }
}

void RenderersManager::updatePerFrameBuffer(Scene& scene)
{
    if (perFrameId_)
    {
        PerFrameBuffer buffer;
        buffer.ProjectionViewMatrix = graphicsApi_.PrepareMatrixToLoad(viewProjectionMatrix_);
        buffer.cameraPosition       = scene.GetCamera().GetPosition();
        buffer.clipPlane            = vec4{0.f, 1.f, 0.f, 100000.f};
        graphicsApi_.UpdateShaderBuffer(*perFrameId_, &buffer);
    }
}
}  // namespace Renderer
}  // namespace GameEngine
