#pragma once
#include <atomic>
#include <list>
#include "GUI/GuiContext.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "IRenderer.h"
#include "Projection.h"
#include "RendererFunctionType.h"

namespace GameEngine
{
class Scene;
class GameObject;
struct GuiTextElement;

namespace Renderer
{
namespace Gui
{
class GuiTextureElement;
}  // Gui

class RenderersManager
{
public:
    RenderersManager(IGraphicsApiPtr graphicsApi);
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

private:
    void ReloadShadersExecution();
    void UpdateCamera(Scene* scene);
    void TakeSnapShoots();
    void InitProjection();
    void InitMainRenderer();
    void InitGuiRenderer();
    void RegisterRenderFunction(RendererFunctionType, RendererFunction);
    void Render(RendererFunctionType type, Scene* scene);

private:
    IGraphicsApiPtr graphicsApi_;
    std::atomic_bool renderAsLines;
    std::atomic_bool markToReloadShaders_;
    Renderer::Gui::GuiContext guiContext_;
    IRenderersPtrVec renderers_;
    Projection projection_;
    std::list<GameObject*> dynamincObjects_;
    RendererFunctions rendererFunctions_;
};
}  // Renderer
}  // GameEngine
