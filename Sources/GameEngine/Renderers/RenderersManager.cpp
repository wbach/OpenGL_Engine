
#include "RenderersManager.h"

#include <Logger/Log.h>

#include <algorithm>
#include <memory>

#include "GUI/GuiRenderer.h"
#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Objects/GameObject.h"
#include "GameEngine/Renderers/BaseRenderer.h"
#include "GameEngine/Renderers/DefferedRenderer.h"
#include "GameEngine/Renderers/Projection/IProjection.h"
#include "GameEngine/Resources/IGpuResourceLoader.h"
#include "GameEngine/Resources/ShaderBuffers/PerFrameBuffer.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "GameEngine/Scene/Scene.hpp"
#include "GraphicsApi/BufferParamters.h"
#include "GraphicsApi/IFrameBuffer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "IRendererFactory.h"
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
GraphicsApi::IFrameBuffer* createPerCameraFrameBuffer(GraphicsApi::IGraphicsApi& api, const vec2ui& framebufferSize,
                                                      const Color& backgroundColor)
{
    using namespace GraphicsApi::FrameBuffer;
    Attachment colorAttachment(framebufferSize, Type::Color0, Format::Rgba8);
    colorAttachment.defaultValue = backgroundColor.value;
    Attachment depthAttachment(framebufferSize, Type::Depth, Format::Depth);
    return &api.CreateFrameBuffer({colorAttachment, depthAttachment});
}
}  // namespace

namespace Renderer
{
RenderersManager::RenderersManager(GraphicsApi::IGraphicsApi& graphicsApi, IGpuResourceLoader& gpuLoader,
                                   Utils::MeasurementHandler& measurmentHandler, Utils::Thread::IThreadSync& threadSync,
                                   const Time& renderThreadTime, std::unique_ptr<IRendererFactory> rendererFactory)
    : graphicsApi_(graphicsApi)
    , gpuLoader_(gpuLoader)
    , measurmentHandler_(measurmentHandler)
    , rendererFactory(std::move(rendererFactory))
    , renderAsLines(false)
    , markToReloadShaders_(false)
    , guiRenderer_(graphicsApi)
    , bufferDataUpdater_()
    , rendererContext_(frustrum_, graphicsApi_, gpuLoader_, measurmentHandler_, renderThreadTime)
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
    LOG_DEBUG << "destructor";

    EngineConf.renderer.shadows.isEnabled.unsubscribe(shadowEnabledSubscriptionId_);
    EngineConf.renderer.viewDistance.unsubscribe(viewDistanceSubscriptionId_);

    if (perFrameId_)
    {
        graphicsApi_.DeleteShaderBuffer(*perFrameId_);
    }
}
void RenderersManager::Init()
{
    InitGuiRenderer();
    debugRenderer_.init();
    CreatePerAppBuffer();
    createMainRenderer();
}
void RenderersManager::createMainRenderer()
{
    LOG_DEBUG << "createMainRenderer";
    graphicsApi_.EnableCulling();
    mainCameraRenderer_ = rendererFactory->create(rendererContext_);

    if (mainCameraRenderer_)
    {
        mainCameraRenderer_->init();
    }
    else
    {
        LOG_ERROR << "Main renderer creation error!";
    }
}
void RenderersManager::InitGuiRenderer()
{
    guiRenderer_.Init();
}
void RenderersManager::renderScene(Scene& scene)
{
    ReloadShadersExecution();
    bufferDataUpdater_.Update();

    cleanupCamerasRenderersIfCameraNotExist(scene);

    auto& cameras           = scene.GetCameraManager().GetActiveCameras();
    rendererContext_.scene_ = &scene;

    uint64 frustrumCheckInFrame = 0;
    ICamera* mainCameraPtr{nullptr};

    for (auto& [_, cameraPtr] : cameras)
    {
        if (scene.GetCameraManager().GetMainCamera() == cameraPtr)
        {
            mainCameraPtr = cameraPtr;
            continue;
        }

        auto rendererContextIter = camerasRenderers.find(cameraPtr);
        if (rendererContextIter != camerasRenderers.end())
        {
            auto& context = rendererContextIter->second;
            if (context.renderSize != cameraPtr->GetProjection().GetRenderingSize())
            {
                if (context.renderTarget)
                {
                    LOG_DEBUG << "Percamera resolution changed from: " << context.renderSize << " => "
                              << cameraPtr->GetProjection().GetRenderingSize();

                    context.renderSize = cameraPtr->GetProjection().GetRenderingSize();
                    rendererContext_.graphicsApi_.DeleteFrameBuffer(*context.renderTarget);
                    context.renderTarget = createPerCameraFrameBuffer(
                        rendererContext_.graphicsApi_, cameraPtr->GetProjection().GetRenderingSize(), scene.GetBackgroundColor());
                    auto status = context.renderTarget->Init();
                    if (not status)
                    {
                        LOG_WARN << "Framebuffer creation error";
                        continue;
                    }
                }
            }

            frustrumCheckInFrame += renderPerCamera(*context.renderer, cameraPtr, context.renderTarget);
        }
        else
        {
            if (auto newContext = createPerCameraRenderer(scene, *cameraPtr))
            {
                auto& context = *newContext;
                frustrumCheckInFrame += renderPerCamera(*context.renderer, cameraPtr, context.renderTarget);
            }
        }
    }

    if (mainCameraRenderer_ and mainCameraPtr)
    {
        frustrumCheckInFrame += renderPerCamera(*mainCameraRenderer_, mainCameraPtr);

        debugRenderer_.render();
        guiRenderer_.render();
    }

    if (not camerasRenderers.empty())
    {
        rendererContext_.graphicsApi_.DisableDepthTest();
        std::vector<GraphicsApi::ID> textures;
        textures.reserve(camerasRenderers.size());

        for (const auto& [camera, cameraContext] : camerasRenderers)
        {
            if (not cameraContext.isVisible or not scene.GetCameraManager().IsCameraActive(camera))
                continue;

            if (not cameraContext.renderTarget)
            {
                continue;
            }
            if (auto id = cameraContext.renderTarget->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0))
            {
                textures.push_back(*id);
            }
        }
        debugRenderer_.renderTextures(textures);
    }

    // debugRenderer_.renderTextures(
    //     {rendererContext_.sharedTextures[magic_enum::enum_index(SharedTextures::shadowCascade0).value()],
    //      rendererContext_.sharedTextures[magic_enum::enum_index(SharedTextures::shadowCascade1).value()],
    //      rendererContext_.sharedTextures[magic_enum::enum_index(SharedTextures::shadowCascade2).value()],
    //      rendererContext_.sharedTextures[magic_enum::enum_index(SharedTextures::shadowCascade3).value()]});

    *frustrumCheckCount_ = std::to_string(frustrumCheckInFrame);

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

    if (mainCameraRenderer_)
        mainCameraRenderer_->reloadShaders();

    for (auto& [_, context] : camerasRenderers)
    {
        if (context.renderer)
            context.renderer->reloadShaders();
    }

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

    if (mainCameraRenderer_)
        mainCameraRenderer_->subscribe(*gameObject);

    for (auto& [_, context] : camerasRenderers)
    {
        if (context.renderer)
            context.renderer->subscribe(*gameObject);
    }

    subscribedGameObjects.insert(gameObject);
}
void RenderersManager::UnSubscribe(GameObject* gameObject)
{
    if (gameObject)
    {
        debugRenderer_.unSubscribe(*gameObject);
        bufferDataUpdater_.UnSubscribe(gameObject);

        if (mainCameraRenderer_)
        {
            mainCameraRenderer_->unSubscribe(*gameObject);
        }

        for (auto& [_, context] : camerasRenderers)
        {
            if (context.renderer)
                context.renderer->unSubscribe(*gameObject);
        }
        subscribedGameObjects.erase(gameObject);
    }
}

void RenderersManager::UnSubscribe(const Components::IComponent& component)
{
    if (mainCameraRenderer_)
        mainCameraRenderer_->unSubscribe(component);

    for (auto& [_, context] : camerasRenderers)
    {
        if (context.renderer)
            context.renderer->unSubscribe(component);
    }
}

void RenderersManager::UnSubscribeAll()
{
    debugRenderer_.unSubscribeAll();

    if (mainCameraRenderer_)
        mainCameraRenderer_->unSubscribeAll();

    for (auto& [_, context] : camerasRenderers)
    {
        if (context.renderer)
            context.renderer->unSubscribeAll();
    }
    bufferDataUpdater_.UnSubscribeAll();
    guiRenderer_.UnSubscribeAll();
    subscribedGameObjects.clear();
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
void RenderersManager::CreatePerAppBuffer()
{
    if (not perAppId_)
        perAppId_ = graphicsApi_.CreateShaderBuffer(PER_APP_BIND_LOCATION, sizeof(PerAppBuffer), GraphicsApi::DrawFlag::Static);

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

        perApp_.useTextures     = vec4(F(textureConfig.useDiffuse), F(textureConfig.useNormal), F(textureConfig.useSpecular),
                                       F(textureConfig.useDisplacement));
        perApp_.shadowVariables = vec4(F(*shadowsConfig.isEnabled), *shadowsConfig.distance, *shadowsConfig.mapSize, 0.f);
        perApp_.viewDistance    = vec4(*rendererConfig.viewDistance, *rendererConfig.normalMappingDistance,
                                       *floraConfig.viewDistance, *rendererConfig.viewDistance);

        perApp_.fogData = vec4(rendererContext_.fogColor_, 3.5f);
        graphicsApi_.UpdateShaderBuffer(*perAppId_, &perApp_);
        graphicsApi_.BindShaderBuffer(*perAppId_);

        LOG_DEBUG << "UpdatePerAppBuffer";
    }
}

void RenderersManager::updatePerFrameBuffer(ICamera& camera)
{
    if (not perFrameId_)
    {
        perFrameId_ =
            graphicsApi_.CreateShaderBuffer(PER_FRAME_BIND_LOCATION, sizeof(PerFrameBuffer), GraphicsApi::DrawFlag::Dynamic);
    }

    if (perFrameId_)
    {
        PerFrameBuffer buffer;
        buffer.ProjectionViewMatrix = graphicsApi_.PrepareMatrixToLoad(camera.GetProjectionViewMatrix());
        buffer.cameraPosition       = camera.GetPosition();
        buffer.clipPlane            = vec4{0.f, 1.f, 0.f, 100000.f};
        buffer.projection           = camera.GetProjection().GetBufferParams();
        graphicsApi_.UpdateShaderBuffer(*perFrameId_, &buffer);
        graphicsApi_.BindShaderBuffer(*perFrameId_);
    }
}
RendererContext& RenderersManager::GetContext()
{
    return rendererContext_;
}
RenderersManager::CameraRendererContext* RenderersManager::createPerCameraRenderer(const Scene& scene, ICamera& camera)
{
    auto framebufferSize = camera.GetProjection().GetRenderingSize();
    CameraRendererContext newCameraRendererContext;
    newCameraRendererContext.renderSize = framebufferSize;
    newCameraRendererContext.renderTarget =
        createPerCameraFrameBuffer(rendererContext_.graphicsApi_, framebufferSize, scene.GetBackgroundColor());
    auto status = newCameraRendererContext.renderTarget->Init();
    if (not status)
    {
        LOG_WARN << "Framebuffer creation error";
        return nullptr;
    }

    newCameraRendererContext.renderer = std::make_unique<BaseRenderer>(rendererContext_);
    newCameraRendererContext.renderer->init();

    for (auto ptr : subscribedGameObjects)
    {
        if (ptr)
        {
            newCameraRendererContext.renderer->subscribe(*ptr);
        }
    }

    camerasRenderers.insert({&camera, std::move(newCameraRendererContext)});
    LOG_DEBUG << "Create buffer and renderer for camera. Rendering size: " << framebufferSize;
    return &camerasRenderers.at(&camera);
}
uint64 RenderersManager::renderPerCamera(BaseRenderer& renderer, ICamera* cameraPtr, GraphicsApi::IFrameBuffer* renderTarget)
{
    rendererContext_.camera_ = cameraPtr;

    cameraPtr->Update();
    cameraPtr->UpdateImpl();
    cameraPtr->UpdateMatrix();

    frustrum_.prepareFrame(cameraPtr->GetProjectionViewMatrix());
    updatePerFrameBuffer(*cameraPtr);

    renderer.setRenderTarget(renderTarget);
    renderer.prepare();
    {
        RenderAsLine lineMode(graphicsApi_, renderAsLines.load());
        graphicsApi_.EnableDepthTest();
        renderer.render();
        renderer.blendRender();
    }

    return frustrum_.getIntersectionsCountInFrame();
}
void RenderersManager::cleanupCamerasRenderersIfCameraNotExist(const Scene& scene)
{
    auto& cameraManager = scene.GetCameraManager();

    const auto& allcameras = cameraManager.GetCameras();
    for (auto iter = camerasRenderers.begin(); iter != camerasRenderers.end();)
    {
        auto allcamerasIter = allcameras.find(iter->first);
        if (allcamerasIter == allcameras.end())
        {
            LOG_DEBUG << "Remove unused camera renderer";
            auto& [renderer, framebuffer, renderingSize, _] = iter->second;
            renderer->unSubscribeAll();
            renderer->cleanUp();
            renderer.reset();

            if (framebuffer)
            {
                rendererContext_.graphicsApi_.DeleteFrameBuffer(*framebuffer);
            }

            iter = camerasRenderers.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}
GraphicsApi::ID RenderersManager::getCameraTexture(ICamera& camera) const
{
    auto iter = camerasRenderers.find(&camera);
    if (iter != camerasRenderers.end())
    {
        return iter->second.renderTarget->GetAttachmentTexture(GraphicsApi::FrameBuffer::Type::Color0);
    }

    return std::nullopt;
}
void RenderersManager::setAdditionalCameraVisiblity(ICamera& camera, bool v)
{
    auto iter = camerasRenderers.find(&camera);
    if (iter != camerasRenderers.end())
    {
        iter->second.isVisible = v;
    }
}
void RenderersManager::hideAdditionalCamerasPreview()
{
    for (auto& [_, context] : camerasRenderers)
    {
        context.isVisible = false;
    }
}
}  // namespace Renderer
}  // namespace GameEngine
