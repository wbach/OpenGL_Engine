#pragma once
#include <atomic>
#include <list>
#include "BufferDataUpdater.h"
#include "GUI/GuiContext.h"
#include "GUI/GuiRenderer.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "IRenderer.h"
#include "Projection.h"
#include "RendererFunctionType.h"
#include <functional>

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
public:
    RenderersManager(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory);
    void Init();
    const Projection& GetProjection() const;
    void RenderScene(Scene* scene, const Time& threadTime);
    void ReloadShaders();
    void Subscribe(GameObject* gameObject);
    void UnSubscribe(GameObject* gameObject);
    void UnSubscribeAll();
    void UnSubscribeAll(std::function<void()>);
    void SwapLineFaceRender();

    void SetPhysicsDebugDraw(std::function<void(const mat4&, const mat4&)> func_);
    void EnableDrawPhysicsDebyg();
    void DisableDrawPhysicsDebyg();
    GUIRenderer& GetGuiRenderer();

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
    std::function<void(const mat4&, const mat4&)> physicsDebugDraw_;
    std::atomic_bool renderAsLines;
    std::atomic_bool markToReloadShaders_;
    std::function<void()> unsubscribeCallback_;

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
