#pragma once
#include <atomic>
#include <list>
#include "GUI/GuiContext.h"
#include "GameEngine/Api/IGraphicsApi.h"
#include "IRenderer.h"
#include "Projection.h"
#include "RendererFunctionType.h"

class CGameObject;
struct SGuiTextElement;

namespace GameEngine
{
class Scene;
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
    const CProjection& GetProjection() const;
    void RenderScene(Scene* scene);
    void ReloadShaders();
    void Subscribe(CGameObject* gameObject);
    void UnSubscribe(CGameObject* gameObject);
    void UnSubscribeAll();
    void SwapLineFaceRender();
    SGuiTextElement& GuiText(const std::string& label);
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
    CProjection projection_;
    std::list<CGameObject*> dynamincObjects_;
    RendererFunctions rendererFunctions_;
};
}  // Renderer
}  // GameEngine
