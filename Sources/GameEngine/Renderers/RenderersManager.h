#pragma once
#include <atomic>
#include <list>
#include "BufferDataUpdater.h"
#include "GUI/GuiRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "DebugElements/DebugRenderer.h"
#include "IRenderer.h"
#include "Projection.h"
#include "RendererFunctionType.h"
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
    RenderersManager(GraphicsApi::IGraphicsApi&, Utils::MeasurementHandler&, Utils::Thread::ThreadSync&);
    ~RenderersManager();
    void Init();
    const Projection& GetProjection() const;
    void RenderScene(Scene* scene, const Time& threadTime);
    void ReloadShaders();
    void Subscribe(GameObject* gameObject);
    void UnSubscribe(GameObject* gameObject);

    void UnSubscribeAll();
    void UnSubscribeAll(std::function<void()>);
    void SwapLineFaceRender();

    GUIRenderer& GetGuiRenderer();
    DebugRenderer& GetDebugRenderer();
    bool IsTesselationSupported() const;

private:
    void ReloadShadersExecution();
    void UpdateCamera(Scene* scene);
    void InitProjection();
    void InitMainRenderer();
    void InitGuiRenderer();
    void RegisterRenderFunction(RendererFunctionType, RendererFunction);
    void Render(RendererFunctionType type, Scene* scene, const Time& threadTime);
    void CreateBuffers();
    void CreatePerAppBuffer();
    void CreatePerFrameBuffer();
    void UpdatePerFrameBuffer(Scene* scene);

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    Utils::MeasurementHandler& measurmentHandler_;
    std::unique_ptr<IFrameBuffer> defferedFrameBuffer_;
    std::unique_ptr<IShadowFrameBuffer> shadowsFrameBuffer_;
    std::unique_ptr<RendererContext> rendererContext_;

    Frustrum frustrum_;
    Projection projection_;
    std::atomic_bool renderAsLines;
    std::atomic_bool markToReloadShaders_;
    std::function<void()> unsubscribeAllCallback_;

    IRenderersPtrVec renderers_;
    GUIRenderer guiRenderer_;
    DebugRenderer debugRenderer_;
    RendererFunctions rendererFunctions_;

    GraphicsApi::ID perFrameId_;
    mat4 viewProjectionMatrix_;
    BufferDataUpdater bufferDataUpdater_;
};
}  // namespace Renderer
}  // namespace GameEngine
