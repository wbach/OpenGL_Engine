#include "Layout.h"
#include <algorithm>

namespace GameEngine
{
Layout::Layout(GuiElementTypes type, const vec2ui& windowSize)
    : GuiElement(type, windowSize)
{
}

void Layout::Remove(GuiElement* element)
{
    std::remove_if(elements_.begin(), elements_.end(), [element](GuiElement* e) { return e->GetId() == element->GetId(); });
    //std::remove_if(children_.begin(), children_.end(), [element](const LayoutElementWrapper& e) { return e.GetId() == element->GetId(); });
}

void Layout::Remove(uint32 id)
{
    std::remove_if(elements_.begin(), elements_.end(), [id](GuiElement* e) { return e->GetId() == id; });
   // std::remove_if(children_.begin(), children_.end(), [id](const LayoutElementWrapper& e) { return e.GetId() == id; });
}

void Layout::SetScale(const vec2& scale)
{
    GuiElement::SetScale(scale);
}

void Layout::Show(bool b)
{
    for (auto& child : children_)
    {
        child.Show(b);
    }
    GuiElement::Show(b);
}

void Layout::Show()
{
    for (auto& child : children_)
    {
        child.Show();
    }
    GuiElement::Show();
}

void Layout::Hide()
{
    for (auto& child : children_)
    {
        child.Hide();
    }
    GuiElement::Hide();
}
}  // namespace GameEngine
