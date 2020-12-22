#include "ContextMenu.h"

namespace GameEngine
{
GuiElementTypes ContextMenu::type = GuiElementTypes::EditBox;

ContextMenu::ContextMenu()
    : GuiElement(type)
{
}
void ContextMenu::Update()
{
}
}  // namespace GameEngine
