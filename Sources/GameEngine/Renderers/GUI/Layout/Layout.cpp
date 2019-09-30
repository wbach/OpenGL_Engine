#include "Layout.h"
#include <Logger/Log.h>
#include <algorithm>

namespace GameEngine
{
Layout::Layout(GuiElementTypes type, const vec2ui& windowSize)
    : GuiElement(type, windowSize)
    , algin_(Algin::CENTER)
{
}

Layout::~Layout()
{
    RemoveAll();
}

void Layout::Update()
{
    for (auto& child : children_)
    {
        child->Update();
    }
}

void Layout::ResetView()
{
}

LayoutElementWrapper& Layout::AddChild(std::unique_ptr<GuiElement> element, std::function<void()> onChange)
{
    element->Show(IsShow());
    element->SetZPositionOffset(GetZTotalValue());
    children_.push_back(std::make_unique<LayoutElementWrapper>(std::move(element), onChange));
    onChange();
    return *children_.back();
}

void Layout::Remove(GuiElement* element)
{
    Remove(element->GetId());
}

void Layout::Remove(uint32 id)
{
    auto it = std::find_if(children_.begin(), children_.end(), [id](const std::unique_ptr<LayoutElementWrapper>& e) { return e->GetId() == id; });
    children_.erase(it);
}

void Layout::RemoveAll()
{
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
        child->Show(b);
    }
    GuiElement::Show(b);
}

void Layout::Show()
{
    for (auto& child : children_)
    {
        child->Show();
    }
    GuiElement::Show();
}

void Layout::Hide()
{
    for (auto& child : children_)
    {
        child->Hide();
    }
    GuiElement::Hide();
}

void Layout::SetRect(const Rect& rect)
{
    for (auto& child : children_)
    {
        child->SetRect(rect);
    }
    GuiElement::SetRect(rect);
}

void Layout::SetPostion(const vec2& position)
{
    for (auto& child : children_)
    {
        child->SetPostion(position);
    }
    GuiElement::SetPostion(position);
}

void Layout::SetPostion(const vec2ui& position)
{
    for (auto& child : children_)
    {
        child->SetPostion(position);
    }
    GuiElement::SetPostion(position);
}

void Layout::SetAlgin(Algin algin)
{
    algin_ = algin;
}

Layout::Algin Layout::GetAlgin() const
{
    return algin_;
}

void Layout::SetZPosition(float z)
{
    GuiElement::SetZPosition(z);

    for (auto& child : children_)
    {
        child->SetZPositionOffset(GetZTotalValue());
    }
}

void Layout::SetZPositionOffset(float offset)
{
    GuiElement::SetZPositionOffset(offset);

    for (auto& child : children_)
    {
        child->SetZPositionOffset(GetZTotalValue());
    }
}
void Layout::SetIsInternal(bool is)
{
    for (auto& child : children_)
    {
        child->SetIsInternal(is);
    }
    GuiElement::SetIsInternal(is);
}

GuiElement* Layout::GetCollisonElement(const vec2& mousePosition)
{
    for (auto& child : children_)
    {
        auto toReturn = child->GetCollisonElement(mousePosition);

        if (toReturn)
        {
            return toReturn;
        }
    }

    return GuiElement::GetCollisonElement(mousePosition);
}

bool Layout::CompareZValue(const GuiElement& element) const
{
    for (auto& child : children_)
    {
        if (not child->CompareZValue(element))
        {
            return false;
        }
    }
    return GuiElement::CompareZValue(element);
}

GuiElement* Layout::Get(const std::string& label)
{
    for (auto& child : children_)
    {
        auto toReturn = child->Get(label);

        if (toReturn)
        {
            return toReturn;
        }
    }

    return GuiElement::Get(label);
}

const std::vector<std::unique_ptr<LayoutElementWrapper>>& Layout::GetChildren() const
{
    return children_;
}

}  // namespace GameEngine
