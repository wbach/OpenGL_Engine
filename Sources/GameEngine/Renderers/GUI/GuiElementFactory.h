#pragma once
#include <Types.h>
#include <functional>
#include <memory>
#include "GuiElement.h"
#include "GuiTheme.h"
#include "Text/GuiTextFactory.h"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class GuiManager;
class GuiTextFactory;
class IResourceManager;
class GuiButtonElement;
class GuiTextureElement;
class GuiWindowElement;
class GuiTextElement;
class GuiEditBoxElement;
class VerticalLayout;
class HorizontalLayout;

namespace Renderer
{
class RenderersManager;
}  // namespace Renderer

class GuiElementFactory
{
public:
    struct EntryParameters
    {
        GuiManager& guiManager_;
        Input::InputManager& inputManager_;
        IResourceManager& resourceManager_;
        Renderer::RenderersManager& renderersManager_;
    };
    GuiElementFactory(EntryParameters& entryParameters);
    ~GuiElementFactory() = default;
    bool ReadGuiFile(const std::string& filename);
    void SetTheme(const GuiTheme&);
    const GuiTheme& GetTheme() const;

    // GuiText
    GuiTextElement* CreateGuiText(const std::string&, const std::string&, uint32, uint32);
    GuiTextElement* CreateGuiText(const std::string&);
    // GuiTexture
    GuiTextureElement* CreateGuiTexture(const std::string&);
    // GuiWindow
    GuiWindowElement* CreateGuiWindow(const Rect&);
    GuiWindowElement* CreateGuiWindow(const Rect&, const std::string&);
    GuiWindowElement* CreateGuiWindow(const vec2&, const vec2&);
    GuiWindowElement* CreateGuiWindow(const vec2&, const vec2&, const std::string&);
    // GuiButton
    GuiButtonElement* CreateGuiButton(std::function<void(GuiElement&)>);
    GuiButtonElement* CreateGuiButton(const std::string&, std::function<void(GuiElement&)>);
    // GuiEditBox
    GuiEditBoxElement* CreateEditBox();
    GuiEditBoxElement* CreateEditBox(const std::string&, const std::string&, uint32, uint32);
    GuiEditBoxElement* CreateEditBox(GuiTextElement*);
    // GuiLayouts
    VerticalLayout* CreateVerticalLayout();
    HorizontalLayout* CreateHorizontalLayout();

    void CreateMessageBox(const std::string& title, const std::string& message, std::function<void()> = nullptr);

private:
    std::unique_ptr<GuiTextureElement> MakeGuiTexture(const std::string&);

private:
    GuiManager& guiManager_;
    Input::InputManager& inputManager_;
    IResourceManager& resourceManager_;
    Renderer::RenderersManager& renderersManager_;
    const vec2ui& windowSize_;
    GuiTextFactory guiTextFactory_;
    std::function<void(uint32)> unsubscribe_;
    GuiTheme theme_;

private:
    uint32 guiElementCounter_;
};
}  // namespace GameEngine
