#pragma once
#include <functional>
#include <memory>
#include "GuiElement.h"

namespace GameEngine
{
namespace Input
{
class InputManager;
} // namespace Input

class GuiManager;
class GuiTextFactory;
class ResourceManager;

class GuiElementFactory
{
public:
    struct EntryParameters
    {
        GuiTextFactory& guiTextFactory_;
        GuiManager& guiManager_;
        Input::InputManager& inputManager_;
        ResourceManager& resourceManager_;
        std::function<void(GuiElement&)> subscribeForRender_;
        const vec2ui& windowSize_;
    };
    GuiElementFactory(EntryParameters& entryParameters);
    GuiElement* CreateGuiText(const std::string&, const std::string&, const std::string&, uint32, uint32);
    GuiElement* CreateGuiTexture(const std::string&, const std::string&);
    GuiElement* CreateGuiWindow(const std::string&, const Rect&, const std::string&);

private:
    std::unique_ptr<GuiElement> MakeGuiTexture(const std::string&);

private:
    GuiTextFactory& guiTextFactory_;
    GuiManager& guiManager_;
    Input::InputManager& inputManager_;
    ResourceManager& resourceManager_;
    std::function<void(GuiElement&)> subscribeForRender_;
    const vec2ui& windowSize_;
};
}  // namespace GameEngine
