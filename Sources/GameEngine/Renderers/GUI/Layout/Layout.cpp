#include "Layout.h"
#include <Logger/Log.h>
#include <algorithm>

namespace GameEngine
{
Layout::Layout(GuiElementTypes type, const vec2ui& windowSize, std::function<void(uint32)> unsubscribe)
    : GuiElement(type, windowSize)
    , unsubscribe_(unsubscribe)
{
}

Layout::~Layout()
{
    RemoveAll();
}

void Layout::ResetView()
{

}

LayoutElementWrapper& Layout::AddChild(GuiElement* element, std::function<void()> f)
{
    element->SetZPositionOffset(GetZTotalValue());
    element->SetPermamanet(isPermament_);
    children_.emplace_back(*element, f);
    f();
    return children_.back();
}

void Layout::Remove(GuiElement* element)
{
    std::remove_if(children_.begin(), children_.end(),
                   [element](const LayoutElementWrapper& e) { return e.GetId() == element->GetId(); });
}

void Layout::Remove(uint32 id)
{
    std::remove_if(children_.begin(), children_.end(), [id](const LayoutElementWrapper& e) { return e.GetId() == id; });
}

void Layout::RemoveAll()
{
    for (auto element : children_)
    {
        element.MarkToRemove();
    }

    children_.clear();
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

void Layout::SetRect(const Rect &rect)
{
    for (auto& child : children_)
    {
        child.SetRect(rect);
    }
    GuiElement::SetRect(rect);
}

void Layout::SetPostion(const vec2& position)
{
    for (auto& child : children_)
    {
        child.SetPostion(position);
    }
    GuiElement::SetPostion(position);
}

void Layout::SetPostion(const vec2ui& position)
{
    for (auto& child : children_)
    {
        child.SetPostion(position);
    }
    GuiElement::SetPostion(position);
}

void Layout::SetZPosition(float z)
{
    GuiElement::SetZPosition(z);

    for (auto& child : children_)
    {
        child.SetZPositionOffset(GetZTotalValue());
    }
}

void Layout::SetZPositionOffset(float offset)
{
    GuiElement::SetZPositionOffset(offset);

    for (auto& child : children_)
    {
        child.SetZPositionOffset(GetZTotalValue());
    }
}
void Layout::SetPermamanet(bool is)
{
    for (auto& child : children_)
    {
        child.SetPermamanet(is);
    }
    GuiElement::SetPermamanet(is);
}
}  // namespace GameEngine
