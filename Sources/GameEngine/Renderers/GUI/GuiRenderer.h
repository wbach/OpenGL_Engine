#pragma once
#include <memory>
#include <vector>
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Renderers/RendererFunctionType.h"
#include "GuiElement.h"

namespace GameEngine
{
class GUIRenderer : public IRenderer
{
public:
    GUIRenderer(std::function<void(RendererFunctionType, RendererFunction)> rendererFunction);
    virtual ~GUIRenderer()
    {
    }
    virtual void Init() override;
    virtual void UnSubscribeAll() override;
    virtual void ReloadShaders() override;
    // create unique_ptr from ptr
    void AddElement(GuiElement* element);

private:
    void Render(Scene* scene);

private:
    std::vector<GuiElementPtr> guiElements;
};
}  // GameEngine
