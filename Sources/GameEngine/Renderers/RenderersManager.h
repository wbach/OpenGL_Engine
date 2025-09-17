#pragma once
#include <atomic>
#include <functional>
#include <list>
#include <memory>

#include "BufferDataUpdater.h"
#include "DebugElements/DebugRenderer.h"
#include "GUI/GuiRenderer.h"
#include "GameEngine/Camera/Frustrum.h"
#include "GameEngine/Resources/ShaderBuffers/PerAppBuffer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "IRenderer.h"
#include "IRendererFactory.h"
#include "Projection.h"
#include "RendererContext.h"

namespace Utils
{
class MeasurementHandler;
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

namespace Renderer
{
class RenderersManager
{
public:
    RenderersManager(GraphicsApi::IGraphicsApi&, IGpuResourceLoader&, Utils::MeasurementHandler&, Utils::Thread::ThreadSync&,
                     const Time&, std::unique_ptr<IRendererFactory>);

    ~RenderersManager();
    void Init();
    const Projection& GetProjection() const;
    void renderScene(Scene&);
    void ReloadShaders();
    void Subscribe(GameObject* gameObject);
    void UnSubscribe(GameObject* gameObject);

    void UnSubscribeAll();
    void UnSubscribeAll(std::function<void()>);
    void SwapLineFaceRender();
    void setLineRenderMode(bool);
    bool getLineRenderMode() const;

    GUIRenderer& GetGuiRenderer();
    DebugRenderer& GetDebugRenderer();
    bool IsTesselationSupported() const;
    void UpdatePerAppBuffer();

    vec2 convertToNdcPosition(const vec3&) const;
    vec3 convertToNdcPosition2(const vec3&) const;
    vec2 convertToScreenPosition(const vec3&) const;

private:
    void ReloadShadersExecution();
    void InitProjection();
    void createMainRenderer();
    void InitGuiRenderer();
    void CreateBuffers();
    void CreatePerAppBuffer();
    void CreatePerFrameBuffer();
    void updatePerFrameBuffer(Scene&);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    IGpuResourceLoader& gpuLoader_;
    Utils::MeasurementHandler& measurmentHandler_;
    std::unique_ptr<IRendererFactory> rendererFactory;

    Frustrum frustrum_;
    Projection projection_;
    std::atomic_bool renderAsLines;
    std::atomic_bool markToReloadShaders_;
    std::function<void()> unsubscribeAllCallback_;

    std::unique_ptr<IRenderer> mainRenderer_;
    GUIRenderer guiRenderer_;

    GraphicsApi::ID perFrameId_;
    GraphicsApi::ID perAppId_;
    mat4 viewProjectionMatrix_;
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
