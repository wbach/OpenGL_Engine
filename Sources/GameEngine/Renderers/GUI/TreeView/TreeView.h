#pragma once
#include <functional>
#include <unordered_map>
#include "GameEngine/Renderers/GUI/GuiElement.h"

namespace Input
{
class InputManager;
}  // namespace Input

namespace GameEngine
{
class GuiElementFactory;
class GuiButtonElement;
class HorizontalLayout;

class TreeView : public GuiElement
{
public:
    TreeView(GuiElementFactory&, std::function<void(const std::string&)>, const vec2ui& windowSize);
    uint32 Add(const std::string& str, std::optional<uint32> parent);

private:
    std::unique_ptr<HorizontalLayout> CreateNode(const std::string& text);

private:
    GuiElementFactory& elementfactory_;
    std::function<void(const std::string&)> selectedItemAction_;
    std::unordered_map<uint32, HorizontalLayout*> elementMap_;
    uint32 idPool_;
    VerticalLayout* layout_;


public:
    static GuiElementTypes type;
};
}  // namespace GameEngine