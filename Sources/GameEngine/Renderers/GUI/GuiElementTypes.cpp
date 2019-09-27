#include "GuiElementTypes.h"

namespace GameEngine
{
// clang-format off
std::vector<GuiElementTypes> GUI_ELEMENT_TYPES
{
    GuiElementTypes::Text,
    GuiElementTypes::Texture,
    GuiElementTypes::Button,
    GuiElementTypes::Window,
    GuiElementTypes::Checkbox,
    GuiElementTypes::EditBox,
    GuiElementTypes::ComboBox,
    GuiElementTypes::VerticalLayout,
    GuiElementTypes::HorizontalLayout
};
}  // namespace GameEngine

namespace std
{
std::string to_string(GameEngine::GuiElementTypes type)
{
    switch (type)
    {
        case GameEngine::GuiElementTypes::Text:             return "Text";
        case GameEngine::GuiElementTypes::Texture:          return "Texture";
        case GameEngine::GuiElementTypes::Button:           return "Button";
        case GameEngine::GuiElementTypes::Window:           return "Window";
        case GameEngine::GuiElementTypes::Checkbox:         return "Checkbox";
        case GameEngine::GuiElementTypes::EditBox:          return "EditBox";
        case GameEngine::GuiElementTypes::ComboBox:         return "ComboBox";
        case GameEngine::GuiElementTypes::VerticalLayout:   return "VerticalLayout";
        case GameEngine::GuiElementTypes::HorizontalLayout: return "HorizontalLayout";
    }

    return "undefined";
}
// clang-format on
}  // namespace std
