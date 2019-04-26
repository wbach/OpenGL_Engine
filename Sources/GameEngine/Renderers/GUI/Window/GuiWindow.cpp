#include "GuiWindow.h"

namespace GameEngine
{
template <class T>
void UpdateChildPosition(GuiElement& element, const T& v)
{
    auto rect = element.GetRect();
    rect.position.x += v.x;
    rect.position.y += v.y;
    element.SetRect(rect);
}

void GuiWindowElement::AddChild(std::unique_ptr<GuiElement> element)
{
    UpdateChildPosition(*element, rect_.position);
    children_.push_back(std::move(element));
}

void GuiWindowElement::Update()
{
    auto mouseMove = inputManager_.CalcualteMouseMove();

    for (auto& child : children_)
    {
        UpdateChildPosition(*child, mouseMove);
    }
}

}  // namespace GameEngine
