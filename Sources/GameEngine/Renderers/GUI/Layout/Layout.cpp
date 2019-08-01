#include "Layout.h"
#include <algorithm>
#include <Logger/Log.h>

namespace GameEngine
{
Layout::Layout(GuiElementTypes type, const vec2ui& windowSize, std::function<void (uint32)> unsubscribe)
    : GuiElement(type, windowSize)
    , unsubscribe_(unsubscribe)
{
}

Layout::~Layout()
{
    RemoveAll();
}

void Layout::Remove(GuiElement* element)
{
    std::remove_if(elements_.begin(), elements_.end(), [element](GuiElement* e) { return e->GetId() == element->GetId(); });
    std::remove_if(children_.begin(), children_.end(), [element](const LayoutElementWrapper& e) { return e.GetId() == element->GetId(); });
}

void Layout::Remove(uint32 id)
{
    std::remove_if(elements_.begin(), elements_.end(), [id](GuiElement* e) { return e->GetId() == id; });
    std::remove_if(children_.begin(), children_.end(), [id](const LayoutElementWrapper& e) { return e.GetId() == id; });
}

void Layout::RemoveAll()
{
    for(auto element : elements_)
    {
        element->execute(unsubscribe_);
        element->MarkToRemove();
    }

    children_.clear();
    elements_.clear();
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

void Layout::SetPostion(const vec2 &position)
{
    for (auto& child : children_)
    {
        child.SetPostion(position);
    }
    GuiElement::SetPostion(position);
}

void Layout::SetPostion(const vec2ui &position)
{
    for (auto& child : children_)
    {
        child.SetPostion(position);
    }
    GuiElement::SetPostion(position);
}
}  // namespace GameEngine
