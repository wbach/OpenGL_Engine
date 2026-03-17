#pragma once
#include <Types.h>

#include <functional>
#include <memory>
#include <string>

namespace GameEngine
{
class GuiElement;
class GuiTextElement;
class GuiTextureElement;
class GuiWindowElement;
class GuiButtonElement;
class GuiEditBoxElement;
class VerticalLayout;
class HorizontalLayout;
class TreeView;
class Menu;
class File;
class GuiTheme;
enum class GuiWindowStyle;
class IGuiElementFactory
{
public:
    virtual ~IGuiElementFactory() = default;

    virtual bool ReadGuiFile(const File&)    = 0;
    virtual void SetTheme(const GuiTheme&)   = 0;
    virtual const GuiTheme& GetTheme() const = 0;

    virtual std::unique_ptr<GuiElement> CreateGuiElement() const                                   = 0;
    virtual std::unique_ptr<GuiElement> CreateGuiElement(const vec2& pos, const vec2& scale) const = 0;

    virtual std::unique_ptr<GuiTextElement> CreateGuiText(const std::string& text, const std::string& font, uint32 size,
                                                          uint32 color)                         = 0;
    virtual std::unique_ptr<GuiTextElement> CreateGuiText(const std::string& text, uint32 size) = 0;
    virtual std::unique_ptr<GuiTextElement> CreateGuiText(const std::string& text)              = 0;

    virtual std::unique_ptr<GuiTextureElement> CreateGuiTexture(const std::string& texturePath) = 0;

    virtual std::unique_ptr<GuiWindowElement> CreateGuiWindow(GuiWindowStyle style, const vec2& pos, const vec2& scale) = 0;
    virtual std::unique_ptr<GuiWindowElement> CreateGuiWindow(GuiWindowStyle style, const vec2& pos, const vec2& scale,
                                                              const vec4& color)                                        = 0;
    virtual std::unique_ptr<GuiWindowElement> CreateGuiWindow(GuiWindowStyle style, const vec2& pos, const vec2& scale,
                                                              const std::string& texture)                               = 0;

    virtual std::unique_ptr<GuiButtonElement> CreateGuiButton(std::function<void(GuiElement&)> action) = 0;
    virtual std::unique_ptr<GuiButtonElement> CreateGuiButton(const std::string& text,
                                                              std::function<void(GuiElement&)> action) = 0;

    virtual std::unique_ptr<GuiEditBoxElement> CreateEditBox()                               = 0;
    virtual std::unique_ptr<GuiEditBoxElement> CreateEditBox(const std::string& placeholder) = 0;

    virtual std::unique_ptr<VerticalLayout> CreateVerticalLayout()     = 0;
    virtual std::unique_ptr<HorizontalLayout> CreateHorizontalLayout() = 0;

    virtual std::unique_ptr<TreeView> CreateTreeView(std::function<void(GuiElement&)> action) = 0;
    virtual std::unique_ptr<Menu> CreateMenu()                                                = 0;

    virtual void CreateMessageBox(const std::string& title, const std::string& message,
                                  std::function<void()> onConfirm = nullptr) = 0;
};

}  // namespace GameEngine
