#pragma once
#include <atomic>
#include <functional>
#include <memory>
#include <optional>
#include <set>
#include <unordered_map>

#include "BufferDataUpdater.h"
#include "DebugElements/DebugRenderer.h"
#include "GUI/GuiRenderer.h"
#include "GameEngine/Camera/Frustrum.h"
#include "GameEngine/Camera/ICamera.h"
#include "GameEngine/Components/IComponent.h"
#include "GameEngine/Renderers/BaseRenderer.h"
#include "GameEngine/Resources/ShaderBuffers/PerAppBuffer.h"
#include "GraphicsApi/GraphicsApiDef.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "IRenderer.h"
#include "RendererContext.h"

namespace GraphicsApi
{
class IFrameBuffer;
}

namespace Utils
{
class MeasurementHandler;

namespace Thread
{
class IThreadSync;
}
}  // namespace Utils

namespace GameEngine
{
class Scene;
class GameObject;
class GuiTextElement;
class GuiTextureElement;
class IFrameBuffer;
class IShadowFrameBuffer;
class IGpuResourceLoader;
class IRendererFactory;

namespace Renderer
{
class RenderersManager
{
public:
    RenderersManager(GraphicsApi::IGraphicsApi&, IGpuResourceLoader&, Utils::MeasurementHandler&, Utils::Thread::IThreadSync&,
                     const Time&, std::unique_ptr<IRendererFactory>);

    ~RenderersManager();
    void Init();
    void renderScene(Scene&);
    void ReloadShaders();
    void Subscribe(GameObject*);
    void UnSubscribe(GameObject*);
    void UnSubscribe(const Components::IComponent&);

    void UnSubscribeAll();
    void UnSubscribeAll(std::function<void()>);
    void SwapLineFaceRender();
    void setLineRenderMode(bool);
    bool getLineRenderMode() const;

    GUIRenderer& GetGuiRenderer();
    DebugRenderer& GetDebugRenderer();
    bool IsTesselationSupported() const;
    void UpdatePerAppBuffer();

    RendererContext& GetContext();
    GraphicsApi::ID getCameraTexture(ICamera&) const;
    void setAdditionalCameraVisiblity(ICamera&, bool);
    void hideAdditionalCamerasPreview();

private:
    struct CameraRendererContext
    {
        std::unique_ptr<BaseRenderer> renderer;
        GraphicsApi::IFrameBuffer* renderTarget;
        vec2ui renderSize;
        bool isVisible{false};
    };

    void ReloadShadersExecution();
    void createMainRenderer();
    void InitGuiRenderer();
    void CreatePerAppBuffer();
    void updatePerFrameBuffer(ICamera&);
    CameraRendererContext* createPerCameraRenderer(const Scene&, ICamera&);
    uint64 renderPerCamera(BaseRenderer& renderer, ICamera* cameraPtr, GraphicsApi::IFrameBuffer* renderTarget = nullptr);
    void cleanupCamerasRenderersIfCameraNotExist(const Scene&);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    IGpuResourceLoader& gpuLoader_;
    Utils::MeasurementHandler& measurmentHandler_;
    std::unique_ptr<IRendererFactory> rendererFactory;

    Frustrum frustrum_;

    std::atomic_bool renderAsLines;
    std::atomic_bool markToReloadShaders_;
    std::function<void()> unsubscribeAllCallback_;

    std::unique_ptr<BaseRenderer> mainCameraRenderer_;


    std::unordered_map<ICamera*, CameraRendererContext> camerasRenderers;
    std::set<GameObject*> subscribedGameObjects;

    GUIRenderer guiRenderer_;

    GraphicsApi::ID perFrameId_;
    GraphicsApi::ID perAppId_;

    BufferDataUpdater bufferDataUpdater_;
    MeasurementValue* frustrumCheckCount_;

    PerAppBuffer perApp_;
    RendererContext rendererContext_;
    DebugRenderer debugRenderer_;
    IdType shadowEnabledSubscriptionId_;
    IdType viewDistanceSubscriptionId_;
};
}  // namespace Renderer
}  // namespace GameEngine
