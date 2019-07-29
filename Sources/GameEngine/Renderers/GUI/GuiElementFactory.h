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
    void ReadGuiFile(const std::string& filename);
    GuiTextElement* CreateGuiText(const std::string&, const std::string&, const std::string&, uint32, uint32);
    GuiTextureElement* CreateGuiTexture(const std::string&, const std::string&);
    GuiWindowElement* CreateGuiWindow(const std::string&, const Rect&, const std::string&);
    GuiWindowElement* CreateGuiWindow(const std::string&, const vec2&, const vec2&, const std::string&);
    GuiButtonElement* CreateGuiButton(const std::string&, std::function<void()>);
    GuiEditBoxElement* CreateEditBox(const std::string&, const std::string&, const std::string&, uint32, uint32);
    GuiEditBoxElement* CreateEditBox(const std::string&, GuiTextElement*);
    VerticalLayout* CreateVerticalLayout(const std::string&);

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

private:
    uint32 guiElementCounter_;
};
}  // namespace GameEngine
