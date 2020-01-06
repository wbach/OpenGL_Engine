#include "ContextMenu.h"

namespace GameEngine
{
GuiElementTypes ContextMenu::type = GuiElementTypes::EditBox;

ContextMenu::ContextMenu(const vec2ui& windowSize)
    : GuiElement(type, windowSize)
{
}
}  // namespace GameEngine
