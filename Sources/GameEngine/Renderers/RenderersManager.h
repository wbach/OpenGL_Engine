#pragma once
#include <atomic>
#include <functional>
#include <list>

#include "BufferDataUpdater.h"
#include "DebugElements/DebugRenderer.h"
#include "GUI/GuiRenderer.h"
#include "GameEngine/Camera/Frustrum.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "IRenderer.h"
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

    RendererContext rendererContext_;
    DebugRenderer debugRenderer_;
};
}  // namespace Renderer
}  // namespace GameEngine
