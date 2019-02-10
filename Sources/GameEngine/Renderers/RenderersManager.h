#pragma once
#include <atomic>
#include <list>
#include "GUI/GuiContext.h"
#include "GraphicsApi/IGraphicsApi.h"
#include "IRenderer.h"
#include "Projection.h"
#include "RendererFunctionType.h"
#include "BufferDataUpdater.h"

namespace GameEngine
{
class Scene;
class GameObject;
struct GuiTextElement;
class IShaderFactory;

namespace Renderer
{
namespace Gui
{
class GuiTextureElement;
}  // Gui

class RenderersManager
{
public:
    RenderersManager(GraphicsApi::IGraphicsApi& graphicsApi, IShaderFactory& shaderFactory);
    void Init();
    const Projection& GetProjection() const;
    void RenderScene(Scene* scene);
    void ReloadShaders();
    void Subscribe(GameObject* gameObject);
    void UnSubscribe(GameObject* gameObject);
    void UnSubscribeAll();
    void SwapLineFaceRender();
    GuiTextElement& GuiText(const std::string& label);
    Gui::GuiTextureElement& GuiTexture(const std::string& label);
    void SetPhysicsDebugDraw(std::function<void(const mat4&, const mat4&)> func_);
    void EnableDrawPhysicsDebyg();
    void DisableDrawPhysicsDebyg();

private:
    void ReloadShadersExecution();
    void UpdateCamera(Scene* scene);
    void InitProjection();
    void InitMainRenderer();
    void InitGuiRenderer();
    void RegisterRenderFunction(RendererFunctionType, RendererFunction);
    void Render(RendererFunctionType type, Scene* scene);
    void CreateBuffers();
    void CreatePerAppBuffer();
    void CreatePerResizeBuffer();
    void CreatePerFrameBuffer();

private:
    GraphicsApi::IGraphicsApi& graphicsApi_;
    std::function<void(const mat4&, const mat4&)> physicsDebugDraw_;
    std::atomic_bool renderAsLines;
    std::atomic_bool markToReloadShaders_;
    Renderer::Gui::GuiContext guiContext_;
    IRenderersPtrVec renderers_;
    Projection projection_;
    RendererFunctions rendererFunctions_;
    IShaderFactory& shaderFactory_;
    bool renderPhysicsDebug_;
    GraphicsApi::ID perFrameId_;
    BufferDataUpdater bufferDataUpdater_;
};
}  // Renderer
}  // GameEngine
