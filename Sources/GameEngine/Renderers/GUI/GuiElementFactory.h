#pragma once
#include <Types.h>
#include <functional>
#include <memory>
#include "GuiElement.h"
#include "GuiTheme.h"
#include "Text/FontManager.h"
#include "Window/GuiWindowStyle.h"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class GuiManager;
class GUIRenderer;
class IResourceManager;
class GuiButtonElement;
class GuiTextureElement;
class GuiWindowElement;
class GuiTextElement;
class GuiEditBoxElement;
class VerticalLayout;
class HorizontalLayout;
class TreeView;
class File;

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
    ~GuiElementFactory();
    bool ReadGuiFile(const std::string& filename);
    void SetTheme(const GuiTheme&);
    const GuiTheme& GetTheme() const;

    // GuiText
    std::unique_ptr<GuiTextElement> CreateGuiText(const std::string&, const std::string&, uint32, uint32);
    std::unique_ptr<GuiTextElement> CreateGuiText(const std::string&, uint32);
    std::unique_ptr<GuiTextElement> CreateGuiText(const std::string&);
    // GuiTexture
    std::unique_ptr<GuiTextureElement> CreateGuiTexture(const std::string&);
    // GuiWindow
    std::unique_ptr<GuiWindowElement> CreateGuiWindow(GuiWindowStyle, const vec2&, const vec2&);
    std::unique_ptr<GuiWindowElement> CreateGuiWindow(GuiWindowStyle, const vec2&, const vec2&, const vec4&);
    std::unique_ptr<GuiWindowElement> CreateGuiWindow(GuiWindowStyle, const vec2&, const vec2&, const std::string&);
    std::unique_ptr<GuiWindowElement> CreateGuiWindow(GuiWindowStyle, const vec2&, const vec2&, const std::string&, const vec4&);
    // GuiButton
    std::unique_ptr<GuiButtonElement> CreateGuiButton(std::function<void(GuiElement&)>);
    std::unique_ptr<GuiButtonElement> CreateGuiButton(const std::string&, std::function<void(GuiElement&)>);
    std::unique_ptr<GuiButtonElement> CreateGuiButton(const std::string&, std::function<void(GuiElement&)>, const std::string&);
    std::unique_ptr<GuiButtonElement> CreateGuiButton(std::function<void(GuiElement&)>, const std::string&, const std::string&, const std::string&);
    std::unique_ptr<GuiButtonElement> CreateGuiButton(const std::string&, std::function<void(GuiElement&)>, const std::string&, const std::string&, const std::string&);
    // GuiEditBox
    std::unique_ptr<GuiEditBoxElement> CreateEditBox();
    std::unique_ptr<GuiEditBoxElement> CreateEditBox(const std::string&);
    std::unique_ptr<GuiEditBoxElement> CreateEditBox(const std::string&, const std::string&, uint32, uint32);
    std::unique_ptr<GuiEditBoxElement> CreateEditBox(std::unique_ptr<GuiTextElement>);
    // GuiLayouts
    std::unique_ptr<VerticalLayout> CreateVerticalLayout();
    std::unique_ptr<HorizontalLayout> CreateHorizontalLayout();

    std::unique_ptr<TreeView> CreateTreeView(std::function<void(GuiElement&)> action);

    void CreateMessageBox(const std::string& title, const std::string& message, std::function<void()> = nullptr);

private:
    std::unique_ptr<GuiTextureElement> MakeGuiTexture(const File&);
    void CreateWindowBar(GuiWindowStyle, GuiWindowElement&);

private:
    GuiManager& guiManager_;
    GUIRenderer& guiRenderer_;
    Input::InputManager& inputManager_;
    IResourceManager& resourceManager_;
    Renderer::RenderersManager& renderersManager_;
    FontManager fontManger_;
    std::function<bool(const GuiElement&)> isOnTop_;
    GuiTheme theme_;

private:
    uint32 guiElementCounter_;
};
}  // namespace GameEngine
