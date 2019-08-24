#pragma once
#include <Types.h>
#include <functional>
#include <memory>
#include "GuiElement.h"
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
    GuiTextElement* CreateGuiText(const std::string&, const std::string&, uint32, uint32);
    GuiTextureElement* CreateGuiTexture(const std::string&);
    GuiWindowElement* CreateGuiWindow(const Rect&, const std::string&);
    GuiWindowElement* CreateGuiWindow(const vec2&, const vec2&, const std::string&);
    GuiButtonElement* CreateGuiButton(std::function<void()>);
    GuiEditBoxElement* CreateEditBox(const std::string&, const std::string&, uint32, uint32);
    GuiEditBoxElement* CreateEditBox(GuiTextElement*);
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
    std::string lastGuiFileMd5Value_;
    std::function<void(uint32)> unsubscribe_;
    uint32 unLabelId_;

private:
    uint32 guiElementCounter_;
};
}  // namespace GameEngine
