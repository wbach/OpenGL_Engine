#pragma once
#include <atomic>
#include <list>
#include "BufferDataUpdater.h"
#include "GUI/GuiContext.h"
#include "GUI/GuiRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "DebugElements/DebugRenderer.h"
#include "IRenderer.h"
#include "Projection.h"
#include "RendererFunctionType.h"
#include <functional>
#include <condition_variable>

namespace GameEngine
{
class Scene;
class GameObject;
class GuiTextElement;
class IShaderFactory;
class GuiTextureElement;

namespace Renderer
{
class RenderersManager
{
    struct UnsubscribeContex
    {
        GameObject* gameObject;
        std::mutex& mutex;
        std::condition_variable& cv;
        bool& done;
    };

public:
    RenderersManager(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory);
    void Init();
    const Projection& GetProjection() const;
    void RenderScene(Scene* scene, const Time& threadTime);
    void ReloadShaders();
    void Subscribe(GameObject* gameObject);
    void UnSubscribe(GameObject* gameObject);
    void UnSubscribe(GameObject* gameObject, std::mutex&, std::condition_variable&, bool& done);

    void UnSubscribeAll();
    void UnSubscribeAll(std::function<void()>);
    void SwapLineFaceRender();

    void SetPhysicsDebugDraw(std::function<void(const mat4&, const mat4&)>);
    void EnableDrawPhysicsDebyg();
    void DisableDrawPhysicsDebyg();
    GUIRenderer& GetGuiRenderer();
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
    
    std::atomic_bool renderAsLines;
    std::atomic_bool markToReloadShaders_;
    std::function<void()> unsubscribeAllCallback_;
    std::vector<UnsubscribeContex> tounsubscriber_;

   // DebugRenderer debugRenderer_;
    IRenderersPtrVec renderers_;
    GUIRenderer guiRenderer_;
    Projection projection_;
    RendererFunctions rendererFunctions_;
    IShaderFactory& shaderFactory_;

    bool renderPhysicsDebug_;
    GraphicsApi::ID perFrameId_;
    BufferDataUpdater bufferDataUpdater_;
};
}  // namespace Renderer
}  // namespace GameEngine
