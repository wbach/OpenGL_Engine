#pragma once
#include <atomic>
#include <list>
#include "BufferDataUpdater.h"
#include "GUI/GuiRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "DebugElements/DebugRenderer.h"
#include "IRenderer.h"
#include "Projection.h"
#include "GameEngine/Camera/Frustrum.h"
#include <functional>

namespace Utils
{
class MeasurementHandler;
} // namespace Utils

namespace GameEngine
{
class Scene;
class GameObject;
class GuiTextElement;
class GuiTextureElement;
class IFrameBuffer;
class IShadowFrameBuffer;
struct RendererContext;

namespace Renderer
{
class RenderersManager
{
public:
    RenderersManager(GraphicsApi::IGraphicsApi&, Utils::MeasurementHandler&, Utils::Thread::ThreadSync&, const Time&);
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
    void UpdatePerAppBuffer() const;

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
    Utils::MeasurementHandler& measurmentHandler_;
    std::unique_ptr<RendererContext> rendererContext_;

    Frustrum frustrum_;
    Projection projection_;
    std::atomic_bool renderAsLines;
    std::atomic_bool markToReloadShaders_;
    std::function<void()> unsubscribeAllCallback_;

    std::unique_ptr<IRenderer> mainRenderer_;
    GUIRenderer guiRenderer_;
    DebugRenderer debugRenderer_;

    GraphicsApi::ID perFrameId_;
    GraphicsApi::ID perAppId_;
    mat4 viewProjectionMatrix_;
    BufferDataUpdater bufferDataUpdater_;
    MeasurementValue* frustrumCheckCount_;

    const Time& renderThreadTime_;
};
}  // namespace Renderer
}  // namespace GameEngine
