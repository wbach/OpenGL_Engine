#pragma once
#include <string>
#include <vector>

namespace GameEngine
{
enum class GuiElementTypes
{
    Text,
    Texture,
    Button,
    Window,
    Checkbox,
    EditBox,
    ComboBox,
    VerticalLayout,
    HorizontalLayout,
};
extern std::vector<GuiElementTypes> GUI_ELEMENT_TYPES;
}  // namespace GameEngine

namespace std
{
std::string to_string(GameEngine::GuiElementTypes type);
}  // namespace std
